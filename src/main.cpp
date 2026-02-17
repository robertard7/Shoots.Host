#include "server/http_server.h"
#include "server/json_writer.h"
#include "server/logging.h"
#include "shoots_host_version.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

namespace {

constexpr const char* kGitRevision = "unknown";
constexpr const char* kBuildTime = __DATE__ " " __TIME__;
std::atomic<bool> g_shutdown_requested{false};

void OnSignal(int) {
    g_shutdown_requested.store(true);
}

int EnvInt(const char* key, int fallback) {
    const char* v = std::getenv(key);
    if (!v || !*v) {
        return fallback;
    }
    return std::atoi(v);
}

std::size_t EnvSize(const char* key, std::size_t fallback) {
    const char* v = std::getenv(key);
    if (!v || !*v) {
        return fallback;
    }
    return static_cast<std::size_t>(std::strtoull(v, nullptr, 10));
}

std::string EnvString(const char* key, std::string fallback) {
    const char* v = std::getenv(key);
    if (!v || !*v) {
        return fallback;
    }
    return v;
}

bool IsValidLogLevel(const std::string& value) {
    return value == "trace" || value == "debug" || value == "info" || value == "warn" || value == "error";
}

bool IsValidLogFormat(const std::string& value) {
    return value == "text" || value == "json";
}

shoots::host::JsonValue BuildConfigJson(const shoots::host::AppConfig& config,
                                        const std::string& bind,
                                        int port,
                                        const std::string& log_level,
                                        const std::string& log_format) {
    shoots::host::JsonValue out = shoots::host::JsonValue::Object();
    out.Set("api_key_enabled", !config.api_key.empty());
    out.Set("bind", bind);
    out.Set("cors_origin", config.cors_origin);
    out.Set("log_format", log_format);
    out.Set("log_level", log_level);
    out.Set("max_body_bytes", static_cast<long long>(config.max_body_bytes));
    out.Set("max_inflight", static_cast<long long>(config.max_inflight));
    out.Set("port", port);
    out.Set("provider_endpoint", config.provider_endpoint);
    out.Set("req_timeout_ms", static_cast<long long>(config.request_timeout_ms));
    out.Set("shutdown_drain_ms", static_cast<long long>(config.shutdown_drain_ms));
    out.Set("version", SHOOTS_HOST_VERSION);
    return out;
}

} // namespace

int main(int argc, char** argv) {
    int port = EnvInt("SHOOTS_HOST_PORT", 8787);
    std::string bind = EnvString("SHOOTS_HOST_BIND", "127.0.0.1");
    std::string log_level = EnvString("SHOOTS_HOST_LOG_LEVEL", "info");
    std::string log_format = EnvString("SHOOTS_HOST_LOG_FORMAT", "text");
    bool print_config = false;
    bool dry_run = false;

    shoots::host::AppConfig config;
    config.max_body_bytes = EnvSize("SHOOTS_HOST_MAX_BODY_BYTES", 1024 * 1024);
    config.request_timeout_ms = EnvSize("SHOOTS_HOST_REQ_TIMEOUT_MS", 1000);
    config.api_key = EnvString("SHOOTS_HOST_API_KEY", "");
    config.cors_origin = EnvString("SHOOTS_HOST_CORS_ORIGIN", "");
    config.max_inflight = EnvSize("SHOOTS_HOST_MAX_INFLIGHT", 64);
    config.shutdown_drain_ms = EnvSize("SHOOTS_HOST_SHUTDOWN_DRAIN_MS", 2000);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--version") {
            std::cout << "ShootsHost " << SHOOTS_HOST_VERSION << "\n";
            return 0;
        }
        if (arg == "--print-config") {
            print_config = true;
            continue;
        }
        if (arg == "--dry-run") {
            dry_run = true;
            continue;
        }
        if (arg == "--port" && i + 1 < argc) {
            port = std::atoi(argv[++i]);
            continue;
        }
        if (arg == "--bind" && i + 1 < argc) {
            bind = argv[++i];
            continue;
        }
        if (arg == "--log-level" && i + 1 < argc) {
            log_level = argv[++i];
            continue;
        }
        if (arg == "--log-format" && i + 1 < argc) {
            log_format = argv[++i];
            continue;
        }
        if (arg == "--max-body-bytes" && i + 1 < argc) {
            config.max_body_bytes = static_cast<std::size_t>(std::strtoull(argv[++i], nullptr, 10));
            continue;
        }
        if (arg == "--req-timeout-ms" && i + 1 < argc) {
            config.request_timeout_ms = static_cast<std::size_t>(std::strtoull(argv[++i], nullptr, 10));
            continue;
        }
        if (arg == "--api-key" && i + 1 < argc) {
            config.api_key = argv[++i];
            continue;
        }
        if (arg == "--cors-origin" && i + 1 < argc) {
            config.cors_origin = argv[++i];
            continue;
        }
        if (arg == "--max-inflight" && i + 1 < argc) {
            config.max_inflight = static_cast<std::size_t>(std::strtoull(argv[++i], nullptr, 10));
            continue;
        }
        if (arg == "--shutdown-drain-ms" && i + 1 < argc) {
            config.shutdown_drain_ms = static_cast<std::size_t>(std::strtoull(argv[++i], nullptr, 10));
            continue;
        }
        std::cerr << "Unknown argument: " << arg << "\n";
        return 2;
    }

    if (!IsValidLogLevel(log_level)) {
        std::cerr << "Invalid log level: " << log_level << "\n";
        return 2;
    }
    if (!IsValidLogFormat(log_format)) {
        std::cerr << "Invalid log format: " << log_format << "\n";
        return 2;
    }

    config.service_version = SHOOTS_HOST_VERSION;
    config.git_revision = kGitRevision;
    config.build_time = kBuildTime;

    const shoots::host::JsonValue config_json = BuildConfigJson(config, bind, port, log_level, log_format);
    if (print_config) {
        std::cout << config_json.Serialize() << "\n";
        return 0;
    }

    shoots::host::ProviderBridge provider;
    provider.Initialize(config.default_model_id, config.max_body_bytes, config.provider_endpoint);
    std::cout << shoots::host::FormatLogLine(log_format, log_level, "provider.version", provider.BuildVersion()) << "\n";
    std::cout << shoots::host::FormatLogLine(log_format, log_level, "provider.ready", provider.IsReady() ? "true" : "false") << "\n";
    std::cout << shoots::host::FormatLogLine(log_format, log_level, "provider.endpoint", provider.Endpoint()) << "\n";

    if (dry_run) {
        std::cout << shoots::host::FormatLogLine(log_format, log_level, "dry-run.config", config_json.Serialize()) << "\n";
        return provider.IsReady() ? 0 : 1;
    }

    shoots::host::HttpServer srv;
    if (!srv.Start(bind, port, config)) {
        std::cerr << "ShootsHost failed to start\n";
        return 1;
    }

    std::signal(SIGINT, OnSignal);
    std::signal(SIGTERM, OnSignal);

    std::cout << shoots::host::FormatLogLine(log_format, log_level, "host.listen", "http://" + bind + ":" + std::to_string(port)) << "\n";

    std::atomic<bool> server_done{false};
    std::thread server_thread([&srv, &server_done]() {
        srv.BlockingRun();
        server_done.store(true);
    });

    while (!g_shutdown_requested.load() && !server_done.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    if (!server_done.load()) {
        srv.Stop();
    }

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(config.shutdown_drain_ms);
    bool drained = false;
    while (std::chrono::steady_clock::now() < deadline) {
        if (srv.InflightRequests() == 0) {
            drained = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (server_thread.joinable()) {
        server_thread.join();
    }

    if (server_done.load() && !g_shutdown_requested.load()) {
        return 0;
    }
    return drained ? 0 : 2;
}
