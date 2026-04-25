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

    profiler.recordEvent("user001", "member_join", base_time);
    profiler.recordEvent("user001", "member_join", base_time + 60000);
    profiler.recordEvent("user001", "member_join", base_time + 120000);
    profiler.recordEvent("user001", "member_join", base_time + 180000);

    profiler.recordEvent("user002", "member_join", base_time + 300000);

    std::cout << "Events recorded." << std::endl;

    std::cout << "\n[2] Checking event counts..." << std::endl;

    std::cout << "  user001 join events (1hr window): "
             << profiler.getEventCount("user001", "member_join") << std::endl;
    std::cout << "  user001 total events (1hr window): "
             << profiler.getTotalEventCount("user001") << std::endl;
    std::cout << "  user002 join events (1hr window): "
             << profiler.getEventCount("user002", "member_join") << std::endl;

    std::cout << "\n[3] Testing anomaly detection..." << std::endl;

    bool anomaly1 = profiler.isAnomaly("user001", "member_join", 3);
    bool anomaly2 = profiler.isAnomaly("user002", "member_join", 3);
    bool anomaly3 = profiler.isRapidJoin("user001", 5, 600000);

    std::cout << "  user001 join >= 3? " << (anomaly1 ? "YES" : "NO") << std::endl;
    std::cout << "  user002 join >= 3? " << (anomaly2 ? "YES" : "NO") << std::endl;
    std::cout << "  user001 rapid join >= 5? " << (anomaly3 ? "YES" : "NO") << std::endl;

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