#include "job_store.h"

#include <algorithm>

namespace shoots::host {

JobStore::JobStore(std::size_t max_jobs) : max_jobs_(max_jobs == 0 ? 1 : max_jobs) {}

JobRecord JobStore::Create(std::string request_type, std::uint64_t provider_request_id) {
    JobRecord record;
    record.job_id = std::to_string(next_job_id_++);
    record.request_type = std::move(request_type);
    record.provider_request_id = provider_request_id;
    record.created_tick = next_tick_++;
    record.status = JobStatus::kQueued;

    order_.push_back(record.job_id);
    jobs_.insert_or_assign(record.job_id, record);
    EvictIfNeeded();
    return record;
}

bool JobStore::UpdateStatus(const std::string& job_id, JobStatus status) {
    const auto it = jobs_.find(job_id);
    if (it == jobs_.end()) {
        return false;
    }
    it->second.status = status;
    return true;
}

bool JobStore::SetResult(const std::string& job_id, std::string result, bool failed) {
    const auto it = jobs_.find(job_id);
    if (it == jobs_.end()) {
        return false;
    }
    it->second.result = std::move(result);
    it->second.status = failed ? JobStatus::kFailed : JobStatus::kComplete;
    return true;
}

std::optional<JobRecord> JobStore::Get(const std::string& job_id) const {
    const auto it = jobs_.find(job_id);
    if (it == jobs_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::optional<JobRecord> JobStore::FindByProviderRequestId(std::uint64_t request_id) const {
    for (const std::string& job_id : order_) {
        const auto it = jobs_.find(job_id);
        if (it != jobs_.end() && it->second.provider_request_id == request_id) {
            return it->second;
        }
    }
    return std::nullopt;
}

std::optional<JobRecord> JobStore::TakeResult(const std::string& job_id) {
    const auto it = jobs_.find(job_id);
    if (it == jobs_.end() || it->second.result.empty()) {
        return std::nullopt;
    }
    JobRecord taken = it->second;
    it->second.result.clear();
    return taken;
}

std::vector<JobRecord> JobStore::ListRecent(std::size_t limit) const {
    std::vector<JobRecord> out;
    const std::size_t capped_limit = std::min(limit, order_.size());
    out.reserve(capped_limit);

    for (auto it = order_.rbegin(); it != order_.rend() && out.size() < capped_limit; ++it) {
        const auto found = jobs_.find(*it);
        if (found != jobs_.end()) {
            out.push_back(found->second);
        }
    }
    return out;
}

void JobStore::EvictIfNeeded() {
    while (order_.size() > max_jobs_) {
        const std::string oldest = order_.front();
        order_.pop_front();
        jobs_.erase(oldest);
    }
}

const char* ToString(JobStatus status) {
    switch (status) {
    case JobStatus::kQueued: return "QUEUED";
    case JobStatus::kRunning: return "RUNNING";
    case JobStatus::kWaiting: return "WAITING";
    case JobStatus::kComplete: return "COMPLETE";
    case JobStatus::kFailed: return "FAILED";
    }
    return "UNKNOWN";
}

} // namespace shoots::host
