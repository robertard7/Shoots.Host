#include "app_state.h"

namespace shoots::host {

AppState::AppState() : jobs_(config_.max_jobs) {
    provider_.Initialize();
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

} // namespace shoots::host
