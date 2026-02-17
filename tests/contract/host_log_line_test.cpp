#include "server/logging.h"

#include <iostream>
#include <string>

int main() {
    const std::string text = shoots::host::FormatLogLine("text", "info", "host.start", "ready");
    if (text != "[info] host.start: ready") {
        std::cerr << "Text log mismatch\n";
        return 1;
    }

    const std::string json = shoots::host::FormatLogLine("json", "info", "host.start", "ready");
    if (json != "{\"event\":\"host.start\",\"level\":\"info\",\"message\":\"ready\"}") {
        std::cerr << "JSON log mismatch\n";
        return 1;
    }

    std::cout << "Log line format is deterministic\n";
    return 0;
}
