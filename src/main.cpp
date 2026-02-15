#include "http_server.h"
#include <cstdlib>
#include <iostream>

static int env_int(const char* key, int fallback) {
    const char* v = std::getenv(key);
    if (!v || !*v) return fallback;
    return std::atoi(v);
}

int main() {
    const int port = env_int("SHOOTS_HOST_PORT", 8787);

    shoots::host::HttpServer srv;
    if (!srv.Start("0.0.0.0", port)) {
        std::cerr << "ShootsHost failed to start\n";
        return 1;
    }

    std::cout << "ShootsHost listening on http://127.0.0.1:" << port << "\n";
    srv.BlockingRun();
    return 0;
}
