#include <fstream>
#include <iostream>
#include <string>

int main() {
    const struct {
        const char* file;
        const char* expected;
    } snapshots[] = {
        {"/tests/golden/health.json", "{\"ok\":true,\"result\":{\"build_time\":\"unknown\",\"git\":\"unknown\",\"service\":\"ShootsHost\",\"uptime_ms\":0,\"version\":\"0.1.0\"}}\n"},
        {"/tests/golden/models.json", "{\"ok\":true,\"result\":{\"items\":[{\"id\":\"provider-default\",\"name\":\"provider-default\"}]}}\n"},
        {"/tests/golden/templates.json", "{\"ok\":true,\"result\":{\"items\":[{\"description\":\"Default deterministic build template\",\"id\":\"build/default\"}]}}\n"},
        {"/tests/golden/submit-chat.json", "{\"ok\":true,\"result\":{\"jobId\":\"1\",\"requestId\":\"req-000001\"}}\n"},
        {"/tests/golden/submit-tool.json", "{\"ok\":true,\"result\":{\"jobId\":\"2\",\"requestId\":\"req-000002\"}}\n"},
        {"/tests/golden/submit-build.json", "{\"ok\":true,\"result\":{\"jobId\":\"3\",\"requestId\":\"req-000003\"}}\n"},
        {"/tests/golden/readyz-ready.json", "{\"ok\":true,\"result\":{\"endpoint\":\"in-memory://provider-bridge\",\"provider_version\":\"provider-bridge/0.1.0\",\"requestId\":\"req-000001\"}}\n"},
        {"/tests/golden/readyz-not-ready.json", "{\"error\":{\"code\":\"not_ready\",\"details\":{\"field\":\"provider\",\"reason\":\"not_ready\"},\"message\":\"provider bridge is not ready\"},\"ok\":false}\n"},
        {"/tests/golden/status.json", "{\"ok\":true,\"result\":{\"last_error_code\":\"\",\"last_error_message\":\"\",\"provider_endpoint\":\"in-memory://provider-bridge\",\"provider_version\":\"provider-bridge/0.1.0\",\"ready\":true,\"request_sequence\":\"req-000001\"}}\n"},
        {"/tests/golden/error-payload-too-large.json", "{\"error\":{\"code\":\"payload_too_large\",\"details\":{\"field\":\"body\",\"reason\":\"too_large\"},\"message\":\"request body too large\"},\"ok\":false}\n"},
        {"/tests/golden/error-unauthorized.json", "{\"error\":{\"code\":\"unauthorized\",\"details\":{\"field\":\"X-Api-Key\",\"reason\":\"missing_or_invalid\"},\"message\":\"api key is required\"},\"ok\":false}\n"},
        {"/tests/golden/print-config.json", "{\"api_key_enabled\":false,\"bind\":\"127.0.0.1\",\"cors_origin\":\"\",\"log_level\":\"info\",\"max_body_bytes\":1048576,\"port\":8787,\"provider_endpoint\":\"in-memory://provider-bridge\",\"req_timeout_ms\":1000}\n"},
    };

    for (const auto& snapshot : snapshots) {
        const std::string path = std::string(SHOOTS_HOST_SOURCE_DIR) + snapshot.file;
        std::ifstream in(path);
        if (!in) {
            std::cerr << "Missing snapshot: " << path << "\n";
            return 1;
        }
        std::string body((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        if (body != snapshot.expected) {
            std::cerr << "Snapshot mismatch: " << snapshot.file << "\n";
            return 1;
        }
    }

    std::cout << "Golden snapshot files are stable\n";
    return 0;
}
