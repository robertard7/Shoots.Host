#include <fstream>
#include <iostream>
#include <string>

int main() {
    const struct {
        const char* file;
        const char* expected;
    } snapshots[] = {
        {"/tests/golden/health.json", "{\"ok\":true,\"result\":{\"build_time\":\"unknown\",\"git\":\"unknown\",\"service\":\"ShootsHost\",\"uptime_ms\":0,\"version\":\"0.1.0\"}}\n"},
        {"/tests/golden/submit-chat.json", "{\"ok\":true,\"result\":{\"jobId\":\"1\",\"requestId\":\"req-000001\"}}\n"},
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
