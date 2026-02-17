#pragma once

#include "jobs/job_store.h"
#include "provider_bridge/provider_bridge.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>

namespace shoots::host {

struct HostMetrics {
    std::uint64_t requests_total = 0;
    std::uint64_t requests_inflight = 0;
    std::uint64_t errors_total = 0;
    std::uint64_t ready_transitions_total = 0;
};

struct AppConfig {
    std::size_t max_jobs = 256;
    std::size_t max_body_bytes = 1024 * 1024;
    std::size_t max_field_bytes = 4096;
    std::size_t poll_cadence_ms = 25;
    std::string default_model_id = "provider-default";
    std::string service_name = "ShootsHost";
    std::string service_version = "0.1.0";
    std::string git_revision = "unknown";
    std::string build_time = "unknown";
    std::string provider_endpoint = "in-memory://provider-bridge";
    std::size_t request_timeout_ms = 1000;
    std::string api_key;
    std::string cors_origin;
};

class AppState final {
public:
    explicit AppState(AppConfig config = {});

    [[nodiscard]] const AppConfig& Config() const;
    [[nodiscard]] JobStore& Jobs();
    [[nodiscard]] ProviderBridge& Provider();
    [[nodiscard]] std::uint64_t NextRequestSequence();
    [[nodiscard]] long long UptimeMs() const;
    void OnRequestStart();
    void OnRequestEnd(bool is_error);
    void MarkReadyState(bool ready);
    [[nodiscard]] HostMetrics Metrics() const;

private:
    AppConfig config_;
    JobStore jobs_;
    ProviderBridge provider_;
    std::uint64_t next_request_sequence_ = 1;
    std::chrono::steady_clock::time_point start_time_;
    HostMetrics metrics_;
    bool last_ready_state_ = false;
};

} // namespace shoots::host
