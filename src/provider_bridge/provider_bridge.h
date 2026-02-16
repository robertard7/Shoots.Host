#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace shoots::host {

struct ProviderModelMeta {
    std::string id;
    std::string name;
};

struct ProviderTemplateMeta {
    std::string id;
    std::string description;
};

struct ProviderResult {
    std::uint64_t request_id = 0;
    std::string content_type;
    std::string payload;
};

class ProviderBridge final {
public:
    ProviderBridge();

    bool Initialize();

    std::vector<ProviderModelMeta> ListModelsMeta() const;
    std::vector<ProviderTemplateMeta> ListTemplatesMeta() const;

    std::uint64_t SubmitChat(const std::string& payload);
    std::uint64_t SubmitTool(const std::string& payload);
    std::uint64_t SubmitBuild(const std::string& payload);

    void Poll();
    std::optional<ProviderResult> TakeResult(std::uint64_t request_id);

private:
    struct PendingRequest {
        std::uint64_t request_id;
        std::string type;
        std::string payload;
    };

    std::uint64_t next_request_id_ = 1;
    std::vector<PendingRequest> pending_;
    std::vector<ProviderResult> ready_;
};

} // namespace shoots::host
