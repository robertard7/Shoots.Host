#pragma once
#include "server/app_state.h"

#include <cstddef>
#include <string>

namespace shoots::host {

class HttpServer final {
public:
    HttpServer();
    ~HttpServer();

    bool Start(const std::string& bindAddr, int port, AppConfig config = {});
    void BlockingRun();
    void Stop();
    [[nodiscard]] std::size_t InflightRequests() const;

private:
    struct Impl;
    Impl* _impl;
};

} // namespace shoots::host
