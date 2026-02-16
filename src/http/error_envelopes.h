#pragma once

#include "server/json_writer.h"

#include <string>

namespace shoots::host {

struct ErrorDetails {
    std::string field;
    std::string reason;
};

JsonValue MakeErrorEnvelope(const std::string& code, const std::string& message);
JsonValue MakeErrorEnvelope(const std::string& code, const std::string& message, const ErrorDetails& details);

} // namespace shoots::host
