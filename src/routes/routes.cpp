#include "routes.h"

#include "server/app_state.h"
#include "server/json_writer.h"

#include <httplib.h>

namespace shoots::host {

namespace {
void WriteJson(httplib::Response& response, int status_code, const JsonValue& value) {
    response.status = status_code;
    response.set_header("Content-Type", "application/json");
    response.body = value.Serialize();
}
}

void RegisterRoutes(httplib::Server& server, AppState&) {
    server.Get("/health", [](const httplib::Request&, httplib::Response& response) {
        JsonValue body = JsonValue::Object();
        body.Set("ok", true);

        WriteJson(response, 200, body);
    });
}

} // namespace shoots::host
