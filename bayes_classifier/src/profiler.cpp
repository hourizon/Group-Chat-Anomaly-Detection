#include "profiler.hpp"
#include <chrono>

namespace profiler {

BehaviorProfiler::BehaviorProfiler(long window_ms) : window_ms_(window_ms) {}

void BehaviorProfiler::add_event(long long user_id, const std::string& event_type) {
    long current_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    user_event_history_[event_type][user_id].push_back(current_timestamp);
}

bool BehaviorProfiler::is_abnormal(long long user_id, const std::string& event_type) {
    if (user_event_history_.find(event_type) == user_event_history_.end() || 
        user_event_history_[event_type].find(user_id) == user_event_history_[event_type].end()) {
        return false;
    }

    auto& timestamps = user_event_history_[event_type][user_id];
    long current_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    // 清理过期事件
    while (!timestamps.empty() && (current_timestamp - timestamps.front() > window_ms_)) {
        timestamps.pop_front();
    }

    // 简单的阈值判断 (例如：1小时内超过10次)
    if (timestamps.size() > 10) { 
        return true;
    }

    return false;
}

} // namespace profiler
