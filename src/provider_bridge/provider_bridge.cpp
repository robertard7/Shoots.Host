#include "provider_bridge.h"

#include <algorithm>

namespace shoots::host {

ProviderBridge::ProviderBridge() = default;

bool ProviderBridge::Initialize() {
    return true;
}

std::vector<ProviderModelMeta> ProviderBridge::ListModelsMeta() const {
    return {
        {"provider-default", "provider-default"},
    };
}

std::vector<ProviderTemplateMeta> ProviderBridge::ListTemplatesMeta() const {
    return {
        {"build/default", "Default deterministic build template"},
    };
}

std::uint64_t ProviderBridge::SubmitChat(const std::string& payload) {
    pending_.push_back({next_request_id_, "chat", payload});
    return next_request_id_++;
}

std::uint64_t ProviderBridge::SubmitTool(const std::string& payload) {
    pending_.push_back({next_request_id_, "tool", payload});
    return next_request_id_++;
}

std::uint64_t ProviderBridge::SubmitBuild(const std::string& payload) {
    pending_.push_back({next_request_id_, "build", payload});
    return next_request_id_++;
}

void ProviderBridge::Poll() {
    for (const PendingRequest& request : pending_) {
        ProviderResult result;
        result.request_id = request.request_id;
        result.content_type = "application/json";
        result.payload = "{\"type\":\"" + request.type + "\",\"echo\":" + request.payload + "}";
        ready_.push_back(std::move(result));
    }
    pending_.clear();
}

std::optional<ProviderResult> ProviderBridge::TakeResult(std::uint64_t request_id) {
    const auto it = std::find_if(ready_.begin(), ready_.end(), [request_id](const ProviderResult& value) {
        return value.request_id == request_id;
    });
    if (it == ready_.end()) {
        return std::nullopt;
    }

    ProviderResult result = *it;
    ready_.erase(it);
    return result;
}

} // namespace shoots::host
