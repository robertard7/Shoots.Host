#pragma once

#include "jobs/job_store.h"
#include "provider_bridge/provider_bridge.h"

#include <cstddef>

namespace shoots::host {

struct AppConfig {
    std::size_t max_jobs = 256;
    std::size_t max_body_bytes = 1024 * 1024;
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
