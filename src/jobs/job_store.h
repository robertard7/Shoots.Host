#pragma once

#include <cstdint>
#include <deque>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace shoots::host {

enum class JobStatus {
    kQueued,
    kRunning,
    kWaiting,
    kComplete,
    kFailed,
};

struct JobRecord {
    std::string job_id;
    std::string request_type;
    std::uint64_t provider_request_id = 0;
    std::uint64_t created_tick = 0;
    JobStatus status = JobStatus::kQueued;
    std::string result;
};

class JobStore final {
public:
    explicit JobStore(std::size_t max_jobs = 256);

    JobRecord Create(std::string request_type, std::uint64_t provider_request_id);
    bool UpdateStatus(const std::string& job_id, JobStatus status);
    bool SetResult(const std::string& job_id, std::string result, bool failed);

    std::optional<JobRecord> Get(const std::string& job_id) const;
    std::optional<JobRecord> FindByProviderRequestId(std::uint64_t request_id) const;
    std::optional<JobRecord> TakeResult(const std::string& job_id);

    std::vector<JobRecord> ListRecent(std::size_t limit) const;

private:
    void EvictIfNeeded();

    std::size_t max_jobs_;
    std::uint64_t next_job_id_ = 1;
    std::uint64_t next_tick_ = 1;
    std::deque<std::string> order_;
    std::unordered_map<std::string, JobRecord> jobs_;
};

const char* ToString(JobStatus status);

} // namespace shoots::host
