#include "http_server.h"

#include "routes/routes.h"
#include "server/app_state.h"

#include <httplib.h>
#include <memory>

namespace shoots::host {

struct HttpServer::Impl {
    std::unique_ptr<httplib::Server> server;
    std::unique_ptr<AppState> app_state;
    std::string bind;
    int port = 0;
};

HttpServer::HttpServer() : _impl(new Impl()) {}
HttpServer::~HttpServer() { delete _impl; }

bool HttpServer::Start(const std::string& bindAddr, int port, AppConfig config) {
    _impl->server = std::make_unique<httplib::Server>();
    _impl->app_state = std::make_unique<AppState>(std::move(config));
    _impl->bind = bindAddr;
    _impl->port = port;

    RegisterRoutes(*_impl->server, *_impl->app_state);

    return true;
}

void HttpServer::BlockingRun() {
    _impl->server->listen(_impl->bind.c_str(), _impl->port);
}

void HttpServer::Stop() {
    if (_impl->server) {
        _impl->server->stop();
    }
}

std::size_t HttpServer::InflightRequests() const {
    if (!_impl->app_state) {
        return 0;
    }
    return static_cast<std::size_t>(_impl->app_state->Metrics().requests_inflight);
}

} // namespace shoots::host
