#include "routes.h"

#include "http/error_envelopes.h"
#include "server/app_state.h"
#include "server/json_writer.h"

#include <httplib.h>

#include <chrono>
#include <cctype>
#include <optional>
#include <set>
#include <string>
#include <thread>

namespace shoots::host {

namespace {

void WriteJson(httplib::Response& response, int status_code, const JsonValue& value) {
    response.status = status_code;
    response.set_header("Content-Type", "application/json");
    response.body = value.Serialize();
}

JsonValue Ok(JsonValue data) {
    JsonValue body = JsonValue::Object();
    body.Set("data", std::move(data));
    body.Set("ok", true);
    return body;
}

JsonValue JobToJson(const JobRecord& job) {
    JsonValue out = JsonValue::Object();
    out.Set("createdTick", static_cast<long long>(job.created_tick));
    out.Set("jobId", job.job_id);
    out.Set("modelId", job.model_id);
    out.Set("requestId", static_cast<long long>(job.provider_request_id));
    out.Set("requestType", job.request_type);
    out.Set("status", ToString(job.status));
    out.Set("templateId", job.template_id);
    out.Set("toolId", job.tool_id);
    return out;
}

std::set<std::string> ExtractTopLevelKeys(const std::string& body) {
    std::set<std::string> keys;
    bool in_string = false;
    bool escape = false;
    int depth = 0;
    std::string current;
    for (std::size_t i = 0; i < body.size(); ++i) {
        const char c = body[i];
        if (!in_string && c == '{') {
            ++depth;
            continue;
        }
        if (!in_string && c == '}') {
            --depth;
            continue;
        }
        if (c == '"' && !escape) {
            in_string = !in_string;
            if (!in_string && depth == 1) {
                std::size_t j = i + 1;
                while (j < body.size() && std::isspace(static_cast<unsigned char>(body[j])) != 0) {
                    ++j;
                }
                if (j < body.size() && body[j] == ':') {
                    keys.insert(current);
                }
                current.clear();
            }
            continue;
        }
        if (in_string && depth == 1) {
            if (escape) {
                current.push_back(c);
                escape = false;
                continue;
            }
            if (c == '\\') {
                escape = true;
                continue;
            }
            current.push_back(c);
        }
    }
    return keys;
}

bool HasKey(const std::set<std::string>& keys, const std::string& key) {
    return keys.find(key) != keys.end();
}

std::optional<std::string> ExtractStringValue(const std::string& body, const std::string& key) {
    const std::string marker = "\"" + key + "\"";
    const std::size_t key_pos = body.find(marker);
    if (key_pos == std::string::npos) {
        return std::nullopt;
    }

    const std::size_t colon_pos = body.find(':', key_pos + marker.size());
    if (colon_pos == std::string::npos) {
        return std::nullopt;
    }

    const std::size_t first_quote = body.find('"', colon_pos + 1);
    if (first_quote == std::string::npos) {
        return std::nullopt;
    }
    const std::size_t second_quote = body.find('"', first_quote + 1);
    if (second_quote == std::string::npos) {
        return std::nullopt;
    }

    return body.substr(first_quote + 1, second_quote - first_quote - 1);
}

bool ValidateEnvelopeShape(const std::string& body) {
    return !(body.empty() || body.front() != '{' || body.back() != '}');
}

void ApplyProviderResult(AppState& app, const std::string& job_id) {
    const std::optional<JobRecord> current = app.Jobs().Get(job_id);
    if (!current.has_value()) {
        return;
    }
    const std::optional<ProviderResult> provider_result = app.Provider().TakeResult(current->provider_request_id);
    if (provider_result.has_value()) {
        app.Jobs().SetResult(job_id, provider_result->payload, false);
    }
}

void HandleSubmit(AppState& app,
                  const std::string& type,
                  const std::set<std::string>& required_fields,
                  const std::set<std::string>& optional_fields,
                  const httplib::Request& request,
                  httplib::Response& response) {
    if (!ValidateEnvelopeShape(request.body)) {
        WriteJson(response, 400, MakeErrorEnvelope("bad_request", "request body must be a JSON object", {"body", "invalid_shape"}));
        return;
    }
    if (request.body.size() > app.Config().max_body_bytes) {
        WriteJson(response, 400, MakeErrorEnvelope("bad_request", "request body too large", {"body", "too_large"}));
        return;
    }

    const std::set<std::string> keys = ExtractTopLevelKeys(request.body);
    for (const std::string& field : required_fields) {
        if (!HasKey(keys, field)) {
            WriteJson(response, 400, MakeErrorEnvelope("bad_request", "missing required field", {field, "required"}));
            return;
        }
    }
    for (const std::string& key : keys) {
        if (!HasKey(required_fields, key) && !HasKey(optional_fields, key)) {
            WriteJson(response, 400, MakeErrorEnvelope("bad_request", "unknown field", {key, "unknown"}));
            return;
        }
    }

    const std::string model_id = ExtractStringValue(request.body, "modelId").value_or(app.Config().default_model_id);
    if (model_id.size() > app.Config().max_field_bytes) {
        WriteJson(response, 400, MakeErrorEnvelope("bad_request", "modelId too large", {"modelId", "too_large"}));
        return;
    }

    const std::string tool_id = ExtractStringValue(request.body, "toolId").value_or("");
    const std::string template_id = ExtractStringValue(request.body, "templateId").value_or("");

    std::uint64_t request_id = 0;
    if (type == "chat") {
        request_id = app.Provider().SubmitChat(request.body);
    } else if (type == "tool") {
        request_id = app.Provider().SubmitTool(request.body);
    } else {
        request_id = app.Provider().SubmitBuild(request.body);
    }

    JobRecord job = app.Jobs().Create(type, model_id, tool_id, template_id, request_id);
    app.Jobs().UpdateStatus(job.job_id, JobStatus::kRunning);
    app.Provider().Poll();

    JsonValue data = JsonValue::Object();
    data.Set("jobId", job.job_id);
    WriteJson(response, 200, Ok(data));
}

bool ParseTimeoutMs(const httplib::Request& request, std::size_t& timeout_ms) {
    if (!request.has_param("timeout_ms")) {
        timeout_ms = 500;
        return true;
    }

    const std::string raw = request.get_param_value("timeout_ms");
    if (raw.empty()) {
        return false;
    }

    std::size_t value = 0;
    for (const char c : raw) {
        if (c < '0' || c > '9') {
            return false;
        }
        value = value * 10 + static_cast<std::size_t>(c - '0');
    }
    timeout_ms = value;
    return true;
}

} // namespace

void RegisterRoutes(httplib::Server& server, AppState& app) {
    server.Get("/health", [](const httplib::Request&, httplib::Response& response) {
        JsonValue body = JsonValue::Object();
        body.Set("ok", true);
        WriteJson(response, 200, body);
    });

    server.Get("/v1/caps", [&app](const httplib::Request&, httplib::Response& response) {
        const ProviderCapabilities caps = app.Provider().GetCapabilities();

        JsonValue data = JsonValue::Object();
        data.Set("maxPayloadBytes", static_cast<long long>(caps.max_payload_bytes));
        data.Set("supportsBuild", caps.supports_build);
        data.Set("supportsChat", caps.supports_chat);
        data.Set("supportsTool", caps.supports_tool);
        WriteJson(response, 200, Ok(data));
    });

    server.Get("/v1/models", [&app](const httplib::Request&, httplib::Response& response) {
        JsonValue items = JsonValue::Array();
        for (const ProviderModelMeta& model : app.Provider().ListModelsMeta()) {
            JsonValue row = JsonValue::Object();
            row.Set("id", model.id);
            row.Set("name", model.name);
            items.Push(std::move(row));
        }

        JsonValue data = JsonValue::Object();
        data.Set("items", std::move(items));
        WriteJson(response, 200, Ok(data));
    });

    server.Get("/v1/templates", [&app](const httplib::Request&, httplib::Response& response) {
        JsonValue items = JsonValue::Array();
        for (const ProviderTemplateMeta& templ : app.Provider().ListTemplatesMeta()) {
            JsonValue row = JsonValue::Object();
            row.Set("description", templ.description);
            row.Set("id", templ.id);
            items.Push(std::move(row));
        }

        JsonValue data = JsonValue::Object();
        data.Set("items", std::move(items));
        WriteJson(response, 200, Ok(data));
    });

    server.Get("/v1/jobs", [&app](const httplib::Request& request, httplib::Response& response) {
        std::size_t limit = 20;
        if (request.has_param("limit")) {
            const std::string raw_limit = request.get_param_value("limit");
            if (raw_limit.empty()) {
                WriteJson(response, 400, MakeErrorEnvelope("bad_request", "limit must be numeric", {"limit", "invalid"}));
                return;
            }
            limit = 0;
            for (const char c : raw_limit) {
                if (c < '0' || c > '9') {
                    WriteJson(response, 400, MakeErrorEnvelope("bad_request", "limit must be numeric", {"limit", "invalid"}));
                    return;
                }
                limit = (limit * 10) + static_cast<std::size_t>(c - '0');
            }
        }

        JsonValue jobs = JsonValue::Array();
        for (const JobRecord& job : app.Jobs().ListRecent(limit)) {
            jobs.Push(JobToJson(job));
        }

        JsonValue data = JsonValue::Object();
        data.Set("items", std::move(jobs));
        WriteJson(response, 200, Ok(data));
    });

    server.Post("/v1/chat", [&app](const httplib::Request& request, httplib::Response& response) {
        HandleSubmit(app, "chat", {"payload"}, {"modelId"}, request, response);
    });

    server.Post("/v1/tool", [&app](const httplib::Request& request, httplib::Response& response) {
        HandleSubmit(app, "tool", {"payload", "toolId"}, {"modelId"}, request, response);
    });

    server.Post("/v1/build", [&app](const httplib::Request& request, httplib::Response& response) {
        HandleSubmit(app, "build", {"language", "payload", "templateId"}, {"modelId"}, request, response);
    });

    server.Get(R"(/v1/jobs/([0-9]+))", [&app](const httplib::Request& request, httplib::Response& response) {
        const std::string job_id = request.matches[1].str();
        app.Provider().Poll();

        const std::optional<JobRecord> job = app.Jobs().Get(job_id);
        if (!job.has_value()) {
            WriteJson(response, 404, MakeErrorEnvelope("not_found", "job not found", {"jobId", "unknown"}));
            return;
        }

        ApplyProviderResult(app, job_id);
        const std::optional<JobRecord> updated = app.Jobs().Get(job_id);

        JsonValue data = JsonValue::Object();
        data.Set("job", JobToJson(*updated));
        WriteJson(response, 200, Ok(data));
    });

    server.Get(R"(/v1/jobs/([0-9]+)/wait)", [&app](const httplib::Request& request, httplib::Response& response) {
        const std::string job_id = request.matches[1].str();
        const std::optional<JobRecord> job = app.Jobs().Get(job_id);
        if (!job.has_value()) {
            WriteJson(response, 404, MakeErrorEnvelope("not_found", "job not found", {"jobId", "unknown"}));
            return;
        }

        std::size_t timeout_ms = 0;
        if (!ParseTimeoutMs(request, timeout_ms)) {
            WriteJson(response, 400, MakeErrorEnvelope("bad_request", "timeout_ms must be numeric", {"timeout_ms", "invalid"}));
            return;
        }

        std::size_t elapsed = 0;
        while (elapsed < timeout_ms) {
            app.Provider().Poll();
            ApplyProviderResult(app, job_id);

            const std::optional<JobRecord> current = app.Jobs().Get(job_id);
            if (current.has_value() && (current->status == JobStatus::kComplete || current->status == JobStatus::kFailed)) {
                JsonValue data = JsonValue::Object();
                data.Set("job", JobToJson(*current));
                WriteJson(response, 200, Ok(data));
                return;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(app.Config().poll_cadence_ms));
            elapsed += app.Config().poll_cadence_ms;
        }

        const std::optional<JobRecord> current = app.Jobs().Get(job_id);
        JsonValue data = JsonValue::Object();
        data.Set("job", JobToJson(*current));
        WriteJson(response, 200, Ok(data));
    });

    server.Post(R"(/v1/jobs/([0-9]+)/take)", [&app](const httplib::Request& request, httplib::Response& response) {
        const std::string job_id = request.matches[1].str();
        const std::optional<JobRecord> job = app.Jobs().Get(job_id);
        if (!job.has_value()) {
            WriteJson(response, 404, MakeErrorEnvelope("not_found", "job not found", {"jobId", "unknown"}));
            return;
        }

        ApplyProviderResult(app, job_id);
        const std::optional<JobRecord> refreshed = app.Jobs().Get(job_id);
        if (refreshed.has_value() && refreshed->result_taken) {
            WriteJson(response, 409, MakeErrorEnvelope("conflict", "result already taken", {"jobId", "already_taken"}));
            return;
        }

        const std::optional<JobRecord> taken = app.Jobs().TakeResult(job_id);
        if (!taken.has_value()) {
            WriteJson(response, 409, MakeErrorEnvelope("conflict", "result not ready", {"jobId", "not_ready"}));
            return;
        }

        JsonValue data = JsonValue::Object();
        data.Set("job", JobToJson(*taken));
        data.Set("result", taken->result);
        WriteJson(response, 200, Ok(data));
    });
}

} // namespace shoots::host
