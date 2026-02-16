#include "json_writer.h"

#include <iomanip>
#include <sstream>

namespace shoots::host {

JsonValue::JsonValue() : type_(Type::kNull), bool_value_(false) {}
JsonValue::JsonValue(std::nullptr_t) : JsonValue() {}
JsonValue::JsonValue(bool value) : type_(Type::kBool), bool_value_(value) {}
JsonValue::JsonValue(int value) : type_(Type::kNumber), bool_value_(false), scalar_value_(std::to_string(value)) {}
JsonValue::JsonValue(long long value) : type_(Type::kNumber), bool_value_(false), scalar_value_(std::to_string(value)) {}
JsonValue::JsonValue(double value) : type_(Type::kNumber), bool_value_(false) {
    std::ostringstream stream;
    stream << std::setprecision(17) << value;
    scalar_value_ = stream.str();
}
JsonValue::JsonValue(const char* value) : type_(Type::kString), bool_value_(false), scalar_value_(value == nullptr ? "" : value) {}
JsonValue::JsonValue(std::string value) : type_(Type::kString), bool_value_(false), scalar_value_(std::move(value)) {}

JsonValue JsonValue::Array() {
    JsonValue value;
    value.type_ = Type::kArray;
    return value;
}

JsonValue JsonValue::Object() {
    JsonValue value;
    value.type_ = Type::kObject;
    return value;
}

JsonValue& JsonValue::Push(JsonValue value) {
    if (type_ != Type::kArray) {
        type_ = Type::kArray;
        array_value_.clear();
    }
    array_value_.push_back(std::move(value));
    return *this;
}

JsonValue& JsonValue::Set(std::string key, JsonValue value) {
    if (type_ != Type::kObject) {
        type_ = Type::kObject;
        object_value_.clear();
    }
    object_value_.insert_or_assign(std::move(key), std::move(value));
    return *this;
}

std::string JsonValue::Serialize() const {
    return SerializeInternal();
}

std::string JsonValue::Escape(std::string_view input) {
    std::string out;
    out.reserve(input.size() + 8);
    for (const char ch : input) {
        switch (ch) {
        case '\\': out += "\\\\"; break;
        case '"': out += "\\\""; break;
        case '\b': out += "\\b"; break;
        case '\f': out += "\\f"; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
            if (static_cast<unsigned char>(ch) < 0x20U) {
                std::ostringstream hex;
                hex << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(ch));
                out += hex.str();
            } else {
                out += ch;
            }
            break;
        }
    }
    return out;
}

std::string JsonValue::SerializeInternal() const {
    switch (type_) {
    case Type::kNull:
        return "null";
    case Type::kBool:
        return bool_value_ ? "true" : "false";
    case Type::kNumber:
        return scalar_value_;
    case Type::kString:
        return "\"" + Escape(scalar_value_) + "\"";
    case Type::kArray: {
        std::string out = "[";
        bool first = true;
        for (const JsonValue& value : array_value_) {
            if (!first) {
                out += ',';
            }
            first = false;
            out += value.SerializeInternal();
        }
        out += ']';
        return out;
    }
    case Type::kObject: {
        std::string out = "{";
        bool first = true;
        for (const auto& [key, value] : object_value_) {
            if (!first) {
                out += ',';
            }
            first = false;
            out += "\"" + Escape(key) + "\":" + value.SerializeInternal();
        }
        out += '}';
        return out;
    }
    }
    return "null";
}

} // namespace shoots::host
