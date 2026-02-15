#include "http_server.h"
#include <httplib.h>
#include <memory>

namespace shoots::host {

struct HttpServer::Impl {
    std::unique_ptr<httplib::Server> server;
    std::string bind;
    int port = 0;
};

HttpServer::HttpServer() : _impl(new Impl()) {}
HttpServer::~HttpServer() { delete _impl; }

bool HttpServer::Start(const std::string& bindAddr, int port) {
    _impl->server = std::make_unique<httplib::Server>();
    _impl->bind = bindAddr;
    _impl->port = port;

    _impl->server->Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Content-Type", "application/json");
        res.status = 200;
        res.body = R"({"ok":true})";
    });

    return true;
}

void HttpServer::BlockingRun() {
    _impl->server->listen(_impl->bind.c_str(), _impl->port);
}

} // namespace shoots::host
