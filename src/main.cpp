#include "server/http_server.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

constexpr const char* kHostVersion = "0.1.0";
constexpr const char* kGitRevision = "unknown";
constexpr const char* kBuildTime = __DATE__ " " __TIME__;

int EnvInt(const char* key, int fallback) {
    const char* v = std::getenv(key);
    if (!v || !*v) {
        return fallback;
    }
    return std::atoi(v);
}

std::string EnvString(const char* key, std::string fallback) {
    const char* v = std::getenv(key);
    if (!v || !*v) {
        return fallback;
    }
    return v;
}

} // namespace

int main(int argc, char** argv) {
    int port = EnvInt("SHOOTS_HOST_PORT", 8787);
    std::string bind = EnvString("SHOOTS_HOST_BIND", "0.0.0.0");
    std::string log_level = EnvString("SHOOTS_HOST_LOG_LEVEL", "info");

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--version") {
            std::cout << "ShootsHost " << kHostVersion << "\n";
            return 0;
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
        std::cerr << "Unknown argument: " << arg << "\n";
        return 2;
    }

    shoots::host::AppConfig config;
    config.service_version = kHostVersion;
    config.git_revision = kGitRevision;
    config.build_time = kBuildTime;

    shoots::host::HttpServer srv;
    if (!srv.Start(bind, port, config)) {
        std::cerr << "ShootsHost failed to start\n";
        return 1;
    }

    shoots::host::ProviderBridge provider;
    provider.Initialize(config.default_model_id, config.max_body_bytes, config.provider_endpoint);
    std::cout << "provider.version=" << provider.BuildVersion() << "\n";
    std::cout << "provider.ready=" << (provider.IsReady() ? "true" : "false") << "\n";
    std::cout << "provider.endpoint=" << provider.Endpoint() << "\n";

    std::cout << "ShootsHost listening on http://" << bind << ":" << port << " (log-level=" << log_level << ")\n";
    srv.BlockingRun();
    return 0;
}
