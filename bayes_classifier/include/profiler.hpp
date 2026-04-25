#ifndef PROFILER_HPP
#define PROFILER_HPP

#include <string>
#include <map>
#include <deque>
#include <chrono>
#include <vector>
#include <cstdint>

namespace profiler {

struct Event {
    std::string user_id;
    std::string event_type;
    int64_t timestamp;

    Event() : timestamp(0) {}
    Event(const std::string& uid, const std::string& type, int64_t ts)
        : user_id(uid), event_type(type), timestamp(ts) {}
};

struct UserProfile {
    std::deque<Event> events;

    void addEvent(const Event& e);
    void cleanOldEvents(int64_t cutoff_time);
    int countEvents(const std::string& event_type, int64_t window_start) const;
    int countAllEvents(int64_t window_start) const;
};

class BehaviorProfiler {
public:
    BehaviorProfiler(long window_ms = 3600000); // 默认1小时窗口
    void add_event(long long user_id, const std::string& event_type);
    bool is_abnormal(long long user_id, const std::string& event_type);

private:
    // ... 内部实现
    long window_ms_;
    std::map<std::string, std::map<long long, std::deque<long>>> user_event_history_;
};

class BaselineAnalyzer {
public:
    BaselineAnalyzer();

    void addObservation(double value);
    void finalize();

    double getMean() const { return mean_; }
    double getStdDev() const { return std_dev_; }
    double getThreshold(double num_std_dev = 3.0) const;

    bool isAnomalous(double value, double num_std_dev = 3.0) const;

private:
    std::vector<double> observations_;
    double mean_;
    double std_dev_;
    bool finalized_;
};

}

#endif