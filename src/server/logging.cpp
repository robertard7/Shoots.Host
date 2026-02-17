#include "server/logging.h"

#include "server/json_writer.h"

namespace shoots::host {

std::string FormatLogLine(const std::string& format,
                          const std::string& level,
                          const std::string& event,
                          const std::string& message) {
    if (format == "json") {
        JsonValue out = JsonValue::Object();
        out.Set("event", event);
        out.Set("level", level);
        out.Set("message", message);
        return out.Serialize();
    }
    return "[" + level + "] " + event + ": " + message;
}

} // namespace shoots::host
