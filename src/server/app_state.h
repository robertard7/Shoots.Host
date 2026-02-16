#pragma once

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

private:
    AppConfig config_;
};

} // namespace shoots::host
