#ifndef INTEGRATION_HPP
#define INTEGRATION_HPP

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <queue>
#include <chrono>
#include <mutex>
#include <iostream>
#include <sstream>
#include <curl/curl.h>

namespace bot {

struct QQEvent {
    std::string post_type;
    std::string message_type;
    std::string sub_type;
    int64_t message_id;
    std::string user_id;
    std::string group_id;
    std::string message;
    int64_t time;
    std::string raw_json;
};

class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    bool init(const std::string& base_url, const std::string& access_token);
    void cleanup();

    bool get(const std::string& action, std::string& response);
    bool post(const std::string& action, const std::map<std::string, std::string>& params, std::string& response);

    bool sendGroupMessage(int64_t group_id, const std::string& message);
    bool sendPrivateMessage(int64_t user_id, const std::string& message);
    bool setGroupKick(int64_t group_id, int64_t user_id, bool reject_request = false);
    bool setGroupBan(int64_t group_id, int64_t user_id, int64_t duration);
    bool deleteMessage(int64_t message_id);

    bool isConnected() const { return connected_; }

private:
    std::string base_url_;
    std::string access_token_;
    bool connected_;
    CURL* curl_;
    std::mutex mutex_;
};

class EventLoop {
public:
    using MessageHandler = std::function<void(const QQEvent&)>;
    using JoinHandler = std::function<void(const QQEvent&)>;

    EventLoop(const std::string& base_url, const std::string& access_token);

    void start();
    void stop();

    void setMessageHandler(MessageHandler handler);
    void setJoinHandler(JoinHandler handler);

    void pollOnce();

    bool isRunning() const { return running_; }

private:
    QQEvent parseEvent(const std::string& json);

    std::string base_url_;
    std::string access_token_;
    HttpClient http_client_;
    std::atomic<bool> running_;
    std::thread poll_thread_;

    MessageHandler message_handler_;
    JoinHandler join_handler_;
};

class DetectorEngine {
public:
    DetectorEngine();

    bool init(const std::string& model_path, int64_t join_threshold = 5, int64_t join_window_ms = 600000);

    void onMessage(const QQEvent& event);
    void onJoin(const QQEvent& event);

    void setHttpClient(HttpClient* client) { http_client_ = client; }

private:
    bool loadModel(const std::string& model_path);

    int64_t parseUserId(const std::string& id_str) const;
    int64_t parseGroupId(const std::string& id_str) const;

    class BayesClassifier* classifier_;
    class BehaviorProfiler* profiler_;

    HttpClient* http_client_;
    int64_t join_threshold_;
    int64_t join_window_ms_;
    std::mutex mutex_;
};

struct RiskResult {
    bool is_risk;
    double confidence;
    std::string reason;

    RiskResult() : is_risk(false), confidence(0.0) {}
};

}

#endif