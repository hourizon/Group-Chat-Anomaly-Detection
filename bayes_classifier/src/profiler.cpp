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

#include <numeric>
#include <cmath>

namespace profiler {

BaselineAnalyzer::BaselineAnalyzer() : mean_(0.0), std_dev_(0.0), finalized_(false) {}

void BaselineAnalyzer::addObservation(double value) {
    if (!finalized_) {
        observations_.push_back(value);
    }
}

void BaselineAnalyzer::finalize() {
    if (observations_.empty() || finalized_) return;

    double sum = std::accumulate(observations_.begin(), observations_.end(), 0.0);
    mean_ = sum / observations_.size();

    double sq_sum = std::inner_product(observations_.begin(), observations_.end(), observations_.begin(), 0.0);
    std_dev_ = std::sqrt(sq_sum / observations_.size() - mean_ * mean_);

    finalized_ = true;
}

double BaselineAnalyzer::getThreshold(double num_std_dev) const {
    return mean_ + num_std_dev * std_dev_;
}

bool BaselineAnalyzer::isAnomalous(double value, double num_std_dev) const {
    if (!finalized_) return false;
    return value > getThreshold(num_std_dev);
}

}
