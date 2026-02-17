#include "server/request_id.h"

#include <cstdio>

namespace shoots::host {

std::string FormatRequestId(std::uint64_t sequence) {
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "req-%06llu", static_cast<unsigned long long>(sequence));
    return buffer;
}

} // namespace shoots::host
