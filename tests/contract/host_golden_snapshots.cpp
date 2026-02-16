#include <fstream>
#include <iostream>
#include <string>

int main() {
    const struct {
        const char* file;
        const char* expected;
    } snapshots[] = {
        {"tests/golden/health.json", "{\"ok\":true}"},
        {"tests/golden/error-not-found.json", "{\"error\":{\"code\":\"not_found\",\"details\":{\"field\":\"jobId\",\"reason\":\"unknown\"},\"message\":\"job not found\"},\"ok\":false}"},
    };

    for (const auto& snapshot : snapshots) {
        std::ifstream in(snapshot.file);
        if (!in) {
            std::cerr << "Missing snapshot: " << snapshot.file << "\n";
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
