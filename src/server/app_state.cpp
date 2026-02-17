#include "app_state.h"

namespace shoots::host {

AppState::AppState(AppConfig config)
    : config_(std::move(config)), jobs_(config_.max_jobs), start_time_(std::chrono::steady_clock::now()) {
    provider_.Initialize(config_.default_model_id, config_.max_body_bytes, config_.provider_endpoint);
    last_ready_state_ = provider_.IsReady();
}

const AppConfig& AppState::Config() const {
    return config_;
}

JobStore& AppState::Jobs() {
    return jobs_;
}

ProviderBridge& AppState::Provider() {
    return provider_;
}

std::uint64_t AppState::NextRequestSequence() {
    return next_request_sequence_++;
}

long long AppState::UptimeMs() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time_).count();
}

void AppState::OnRequestStart() {
    ++metrics_.requests_total;
    ++metrics_.requests_inflight;
}

void AppState::OnRequestEnd(bool is_error) {
    if (metrics_.requests_inflight > 0) {
        --metrics_.requests_inflight;
    }
    if (is_error) {
        ++metrics_.errors_total;
    }
}

void AppState::MarkReadyState(bool ready) {
    if (ready != last_ready_state_) {
        ++metrics_.ready_transitions_total;
        last_ready_state_ = ready;
    }
}

HostMetrics AppState::Metrics() const {
    return metrics_;
}

} // namespace shoots::host
