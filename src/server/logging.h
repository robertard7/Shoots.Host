#pragma once

#include <string>

namespace shoots::host {

std::string FormatLogLine(const std::string& format,
                          const std::string& level,
                          const std::string& event,
                          const std::string& message);

} // namespace shoots::host
