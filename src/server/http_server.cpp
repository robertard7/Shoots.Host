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

bool HttpServer::Start(const std::string& bindAddr, int port) {
    _impl->server = std::make_unique<httplib::Server>();
    _impl->app_state = std::make_unique<AppState>();
    _impl->bind = bindAddr;
    _impl->port = port;

    RegisterRoutes(*_impl->server, *_impl->app_state);

    return true;
}

void HttpServer::BlockingRun() {
    _impl->server->listen(_impl->bind.c_str(), _impl->port);
}

} // namespace shoots::host
