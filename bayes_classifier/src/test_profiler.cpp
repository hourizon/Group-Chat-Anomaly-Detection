#include "profiler.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace profiler;

int main() {
    std::cout << "=== Behavior Profiler Test ===" << std::endl;

    BehaviorProfiler profiler(600000);

    int64_t base_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    std::cout << "\n[1] Recording events..." << std::endl;

    profiler.add_event(1, "member_join");
    profiler.add_event(1, "member_join");
    profiler.add_event(1, "member_join");
    profiler.add_event(1, "member_join");

    profiler.add_event(2, "member_join");

    std::cout << "Events recorded." << std::endl;

    std::cout << "\n[2] Testing anomaly detection..." << std::endl;

    bool anomaly1 = profiler.is_abnormal(1, "member_join");
    bool anomaly2 = profiler.is_abnormal(2, "member_join");

    std::cout << "  user001 is abnormal? " << (anomaly1 ? "YES" : "NO") << std::endl;
    std::cout << "  user002 is abnormal? " << (anomaly2 ? "YES" : "NO") << std::endl;

    std::cout << "\n[4] Testing baseline analyzer..." << std::endl;

    BaselineAnalyzer baseline;
    baseline.addObservation(1.0);
    baseline.addObservation(2.0);
    baseline.addObservation(3.0);
    baseline.addObservation(4.0);
    baseline.addObservation(5.0);
    baseline.finalize();

    std::cout << "  Observations: [1, 2, 3, 4, 5]" << std::endl;
    std::cout << "  Mean: " << baseline.getMean() << std::endl;
    std::cout << "  StdDev: " << baseline.getStdDev() << std::endl;
    std::cout << "  Threshold (3 std): " << baseline.getThreshold(3.0) << std::endl;

    bool anom1 = baseline.isAnomalous(10.0, 3.0);
    bool anom2 = baseline.isAnomalous(4.5, 3.0);
    std::cout << "  10 anomalous? " << (anom1 ? "YES" : "NO") << std::endl;
    std::cout << "  4.5 anomalous? " << (anom2 ? "YES" : "NO") << std::endl;

    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
}