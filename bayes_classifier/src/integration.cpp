#include "integration.hpp"
#include <iostream>
#include <sstream>
#include <chrono>

namespace bot {

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

HttpClient::HttpClient() : connected_(false), curl_(nullptr) {}

HttpClient::~HttpClient() {
    cleanup();
}

bool HttpClient::init(const std::string& base_url, const std::string& access_token) {
    base_url_ = base_url;
    access_token_ = access_token;

    curl_ = curl_easy_init();
    if (!curl_) {
        std::cerr << "Failed to init curl" << std::endl;
        return false;
    }

    connected_ = true;
    std::cout << "[HttpClient] Initialized with base_url: " << base_url << std::endl;
    return true;
}

void HttpClient::cleanup() {
    if (curl_) {
        curl_easy_cleanup(curl_);
        curl_ = nullptr;
    }
    connected_ = false;
}

bool HttpClient::get(const std::string& action, std::string& response) {
    if (!curl_ || !connected_) return false;

    std::string url = base_url_ + "/" + action;

    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Access-Token: " + access_token_).c_str());
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl_);

    curl_slist_free_all(headers);

    return res == CURLE_OK;
}

bool HttpClient::post(const std::string& action, const std::map<std::string, std::string>& params, std::string& response) {
    if (!curl_ || !connected_) return false;

    std::string url = base_url_ + "/" + action;

    std::string post_data;
    for (auto it = params.begin(); it != params.end(); ++it) {
        if (it != params.begin()) post_data += "&";
        post_data += it->first + "=" + it->second;
    }

    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, ("Access-Token: " + access_token_).c_str());
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl_);

    curl_slist_free_all(headers);

    return res == CURLE_OK;
}

bool HttpClient::sendGroupMessage(int64_t group_id, const std::string& message) {
    std::map<std::string, std::string> params;
    params["group_id"] = std::to_string(group_id);
    params["message"] = message;

    std::string response;
    bool success = post("send_group_msg", params, response);

    if (success) {
        std::cout << "[HttpClient] Sent to group " << group_id << ": " << message << std::endl;
    }
    return success;
}

bool HttpClient::sendPrivateMessage(int64_t user_id, const std::string& message) {
    std::map<std::string, std::string> params;
    params["user_id"] = std::to_string(user_id);
    params["message"] = message;

    std::string response;
    return post("send_private_msg", params, response);
}

bool HttpClient::setGroupKick(int64_t group_id, int64_t user_id, bool reject_request) {
    std::map<std::string, std::string> params;
    params["group_id"] = std::to_string(group_id);
    params["user_id"] = std::to_string(user_id);
    params["reject_add_request"] = reject_request ? "true" : "false";

    std::string response;
    return post("set_group_kick", params, response);
}

bool HttpClient::setGroupBan(int64_t group_id, int64_t user_id, int64_t duration) {
    std::map<std::string, std::string> params;
    params["group_id"] = std::to_string(group_id);
    params["user_id"] = std::to_string(user_id);
    params["duration"] = std::to_string(duration);

    std::string response;
    return post("set_group_ban", params, response);
}

bool HttpClient::deleteMessage(int64_t message_id) {
    std::map<std::string, std::string> params;
    params["message_id"] = std::to_string(message_id);

    std::string response;
    return post("delete_msg", params, response);
}

EventLoop::EventLoop(const std::string& base_url, const std::string& access_token)
    : base_url_(base_url), access_token_(access_token), running_(false) {}

void EventLoop::start() {
    running_ = true;
    poll_thread_ = std::thread([this]() {
        while (running_) {
            pollOnce();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    std::cout << "[EventLoop] Started" << std::endl;
}

void EventLoop::stop() {
    running_ = false;
    if (poll_thread_.joinable()) {
        poll_thread_.join();
    }
    std::cout << "[EventLoop] Stopped" << std::endl;
}

void EventLoop::setMessageHandler(MessageHandler handler) {
    message_handler_ = handler;
}

void EventLoop::setJoinHandler(JoinHandler handler) {
    join_handler_ = handler;
}

void EventLoop::pollOnce() {
    std::string response;
    if (http_client_.get("get_login_info", response)) {
    }
}

QQEvent EventLoop::parseEvent(const std::string& json) {
    QQEvent event;
    return event;
}

DetectorEngine::DetectorEngine()
    : classifier_(nullptr), profiler_(nullptr), http_client_(nullptr),
      join_threshold_(5), join_window_ms_(600000) {}

bool DetectorEngine::init(const std::string& model_path, int64_t join_threshold, int64_t join_window_ms) {
    join_threshold_ = join_threshold;
    join_window_ms_ = join_window_ms;

    std::cout << "[DetectorEngine] Initialized" << std::endl;
    std::cout << "  - Model: " << model_path << std::endl;
    std::cout << "  - Join threshold: " << join_threshold_ << std::endl;
    std::cout << "  - Join window: " << join_window_ms_ << "ms" << std::endl;

    return true;
}

void DetectorEngine::onMessage(const QQEvent& event) {
    if (!classifier_ || !http_client_) return;

    double spam_prob = classifier_->classify(event.message);

    if (spam_prob > 0.95) {
        std::cout << "[DetectorEngine] SPAM detected: " << event.message << std::endl;
        http_client_->deleteMessage(event.message_id);
    }
}

void DetectorEngine::onJoin(const QQEvent& event) {
    if (!profiler_ || !http_client_) return;

    int64_t user_id = parseUserId(event.user_id);
    profiler_->recordEvent(event.user_id, "member_join", event.time);

    if (profiler_->isRapidJoin(event.user_id, join_threshold_, join_window_ms_)) {
        std::cout << "[DetectorEngine] Rapid join detected for user: " << event.user_id << std::endl;
        http_client_->setGroupKick(parseGroupId(event.group_id), user_id, true);
    }
}

int64_t DetectorEngine::parseUserId(const std::string& id_str) const {
    try {
        return std::stoll(id_str);
    } catch (...) {
        return 0;
    }
}

int64_t DetectorEngine::parseGroupId(const std::string& id_str) const {
    try {
        return std::stoll(id_str);
    } catch (...) {
        return 0;
    }
}

}