#include "server/request_id.h"

#include <fstream>
#include <iostream>
#include <string>

int main() {
    const std::string expected = "X-Request-Id: req-000001\n";
    const std::string actual = "X-Request-Id: " + shoots::host::FormatRequestId(1) + "\n";

    if (actual != expected) {
        std::cerr << "Header contract mismatch\n";
        return 1;
    }

    std::ifstream in(std::string(SHOOTS_HOST_SOURCE_DIR) + "/tests/golden/header-request-id.txt");
    if (!in) {
        std::cerr << "Missing header snapshot\n";
        return 1;
    }
    std::string file((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (file != expected) {
        std::cerr << "Header snapshot mismatch\n";
        return 1;
    }

    std::cout << "Header snapshot stable\n";
    return 0;
}
