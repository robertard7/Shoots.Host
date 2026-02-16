#include "http/error_envelopes.h"

namespace shoots::host {

JsonValue MakeErrorEnvelope(const std::string& code, const std::string& message) {
    JsonValue details = JsonValue::Object();
    details.Set("field", "");
    details.Set("reason", "");

    JsonValue error = JsonValue::Object();
    error.Set("code", code);
    error.Set("details", std::move(details));
    error.Set("message", message);

    JsonValue body = JsonValue::Object();
    body.Set("error", std::move(error));
    body.Set("ok", false);
    return body;
}

JsonValue MakeErrorEnvelope(const std::string& code, const std::string& message, const ErrorDetails& details_in) {
    JsonValue details = JsonValue::Object();
    details.Set("field", details_in.field);
    details.Set("reason", details_in.reason);

    JsonValue error = JsonValue::Object();
    error.Set("code", code);
    error.Set("details", std::move(details));
    error.Set("message", message);

    JsonValue body = JsonValue::Object();
    body.Set("error", std::move(error));
    body.Set("ok", false);
    return body;
}

} // namespace shoots::host
