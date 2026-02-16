#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace shoots::host {

class JsonValue final {
public:
    enum class Type {
        kNull,
        kBool,
        kNumber,
        kString,
        kArray,
        kObject,
    };

    JsonValue();
    JsonValue(std::nullptr_t);
    JsonValue(bool value);
    JsonValue(int value);
    JsonValue(long long value);
    JsonValue(double value);
    JsonValue(const char* value);
    JsonValue(std::string value);

    static JsonValue Array();
    static JsonValue Object();

    JsonValue& Push(JsonValue value);
    JsonValue& Set(std::string key, JsonValue value);

    [[nodiscard]] std::string Serialize() const;

private:
    Type type_;
    bool bool_value_;
    std::string scalar_value_;
    std::vector<JsonValue> array_value_;
    std::map<std::string, JsonValue> object_value_;

    [[nodiscard]] static std::string Escape(std::string_view input);
    [[nodiscard]] std::string SerializeInternal() const;
};

} // namespace shoots::host
