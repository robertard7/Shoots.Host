#include "server/http_server.h"

#include <httplib.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace {

bool WaitForServer(int port) {
    for (int i = 0; i < 100; ++i) {
        httplib::Client client("127.0.0.1", port);
        if (auto response = client.Get("/livez")) {
            if (response->status == 200) {
                return true;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    return false;
}

bool CheckMetrics(const shoots::host::AppConfig& config,
                  int port,
                  const std::string& expected_content_type_prefix,
                  const std::string& expected_body_prefix) {
    shoots::host::HttpServer server;
    if (!server.Start("127.0.0.1", port, config)) {
        std::cerr << "Failed to start server" << std::endl;
        return false;
    }

    std::thread server_thread([&server]() { server.BlockingRun(); });

    if (!WaitForServer(port)) {
        std::cerr << "Server failed readiness check" << std::endl;
        server.Stop();
        if (server_thread.joinable()) {
            server_thread.join();
        }
        return false;
    }

    httplib::Client client("127.0.0.1", port);
    const auto response = client.Get("/metrics");

    server.Stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }

    if (!response) {
        std::cerr << "No /metrics response" << std::endl;
        return false;
    }

    if (response->status != 200) {
        std::cerr << "Unexpected /metrics status: " << response->status << std::endl;
        return false;
    }

    const std::string content_type = response->get_header_value("Content-Type");
    if (!content_type.starts_with(expected_content_type_prefix)) {
        std::cerr << "Content-Type mismatch: got '" << content_type << "'" << std::endl;
        return false;
    }

    if (!response->body.starts_with(expected_body_prefix)) {
        std::cerr << "Body prefix mismatch" << std::endl;
        return false;
    }

    return true;
}

} // namespace

int main() {
    shoots::host::AppConfig json_config;
    json_config.metrics_format = "json";
    if (!CheckMetrics(json_config, 18991, "application/json", "{\"ok\":true,")) {
        return 1;
    }

    shoots::host::AppConfig prometheus_config;
    prometheus_config.metrics_format = "prometheus";
    if (!CheckMetrics(prometheus_config,
                      18992,
                      "text/plain; version=0.0.4",
                      "# TYPE requests_total counter\n")) {
        return 1;
    }

    std::cout << "Metrics Content-Type contract stable" << std::endl;
    return 0;
}
