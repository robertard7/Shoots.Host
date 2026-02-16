#pragma once

#include "jobs/job_store.h"
#include "provider_bridge/provider_bridge.h"

#include <cstddef>
#include <string>

namespace shoots::host {

struct AppConfig {
    std::size_t max_jobs = 256;
    std::size_t max_body_bytes = 1024 * 1024;
    std::size_t max_field_bytes = 4096;
    std::size_t poll_cadence_ms = 25;
    std::string default_model_id = "provider-default";
};

class AppState final {
public:
    AppState();

    [[nodiscard]] const AppConfig& Config() const;
    [[nodiscard]] JobStore& Jobs();
    [[nodiscard]] ProviderBridge& Provider();

private:
    AppConfig config_;
    JobStore jobs_;
    ProviderBridge provider_;
};

} // namespace shoots::host
