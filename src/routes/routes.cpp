#include "routes.h"

#include "server/app_state.h"
#include "server/json_writer.h"

#include <httplib.h>

#include <cstdint>
#include <optional>

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

JsonValue Error(const std::string& code, const std::string& message) {
    JsonValue error = JsonValue::Object();
    error.Set("code", code);
    error.Set("message", message);

    JsonValue body = JsonValue::Object();
    body.Set("error", error);
    body.Set("ok", false);
    return body;
}

JsonValue JobToJson(const JobRecord& job) {
    JsonValue out = JsonValue::Object();
    out.Set("createdTick", static_cast<long long>(job.created_tick));
    out.Set("jobId", job.job_id);
    out.Set("requestId", static_cast<long long>(job.provider_request_id));
    out.Set("requestType", job.request_type);
    out.Set("status", ToString(job.status));
    return out;
}

void HandleSubmit(AppState& app, const std::string& type, const httplib::Request& request, httplib::Response& response) {
    std::uint64_t request_id = 0;
    if (type == "chat") {
        request_id = app.Provider().SubmitChat(request.body);
    } else if (type == "tool") {
        request_id = app.Provider().SubmitTool(request.body);
    } else {
        request_id = app.Provider().SubmitBuild(request.body);
    }

    JobRecord job = app.Jobs().Create(type, request_id);
    app.Jobs().UpdateStatus(job.job_id, JobStatus::kRunning);
    app.Provider().Poll();

    JsonValue data = JsonValue::Object();
    data.Set("jobId", job.job_id);
    WriteJson(response, 202, Ok(data));
}
}

void RegisterRoutes(httplib::Server& server, AppState& app) {
    server.Get("/health", [](const httplib::Request&, httplib::Response& response) {
        JsonValue data = JsonValue::Object();
        data.Set("ok", true);
        WriteJson(response, 200, data);
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

    server.Post("/v1/chat", [&app](const httplib::Request& request, httplib::Response& response) {
        HandleSubmit(app, "chat", request, response);
    });

    server.Post("/v1/tool", [&app](const httplib::Request& request, httplib::Response& response) {
        HandleSubmit(app, "tool", request, response);
    });

    server.Post("/v1/build", [&app](const httplib::Request& request, httplib::Response& response) {
        HandleSubmit(app, "build", request, response);
    });

    server.Get(R"(/v1/jobs/([0-9]+))", [&app](const httplib::Request& request, httplib::Response& response) {
        const std::string job_id = request.matches[1].str();
        app.Provider().Poll();

        const std::optional<JobRecord> job = app.Jobs().Get(job_id);
        if (!job.has_value()) {
            WriteJson(response, 404, Error("not_found", "job not found"));
            return;
        }

        const std::optional<ProviderResult> provider_result = app.Provider().TakeResult(job->provider_request_id);
        if (provider_result.has_value()) {
            app.Jobs().SetResult(job_id, provider_result->payload, false);
        }

        const std::optional<JobRecord> updated = app.Jobs().Get(job_id);
        JsonValue data = JsonValue::Object();
        data.Set("job", JobToJson(*updated));
        WriteJson(response, 200, Ok(data));
    });

    server.Post(R"(/v1/jobs/([0-9]+)/take)", [&app](const httplib::Request& request, httplib::Response& response) {
        const std::string job_id = request.matches[1].str();
        const std::optional<JobRecord> job = app.Jobs().Get(job_id);
        if (!job.has_value()) {
            WriteJson(response, 404, Error("not_found", "job not found"));
            return;
        }

        const std::optional<JobRecord> taken = app.Jobs().TakeResult(job_id);
        if (!taken.has_value()) {
            WriteJson(response, 409, Error("not_ready", "result not ready"));
            return;
        }

        JsonValue data = JsonValue::Object();
        data.Set("job", JobToJson(*taken));
        data.Set("result", taken->result);
        WriteJson(response, 200, Ok(data));
    });
}

} // namespace shoots::host
