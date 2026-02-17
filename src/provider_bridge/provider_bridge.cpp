#include "provider_bridge.h"

#include <algorithm>

namespace shoots::host {

ProviderBridge::ProviderBridge() = default;

bool ProviderBridge::Initialize(const std::string& default_model_id, std::uint64_t max_payload_bytes, std::string endpoint) {
    if (initialized_) {
        return true;
    }

    default_model_id_ = default_model_id.empty() ? "provider-default" : default_model_id;
    capabilities_.max_payload_bytes = max_payload_bytes;
    endpoint_ = endpoint.empty() ? "in-memory://provider-bridge" : std::move(endpoint);
    initialized_ = true;
    return true;
}

std::vector<ProviderModelMeta> ProviderBridge::ListModelsMeta() const {
    return {
        {default_model_id_, default_model_id_},
    };
}

std::vector<ProviderTemplateMeta> ProviderBridge::ListTemplatesMeta() const {
    return {
        {"build/default", "Default deterministic build template"},
    };
}

ProviderCapabilities ProviderBridge::GetCapabilities() const {
    return capabilities_;
}

bool ProviderBridge::IsReady() const {
    return initialized_;
}

std::string ProviderBridge::BuildVersion() const {
    return "provider-bridge/0.1.0";
}

std::string ProviderBridge::Endpoint() const {
    return endpoint_;
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
        result.payload = "{\"requestId\":" + std::to_string(request.request_id) + ",\"type\":\"" + request.type + "\"}";
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
