#ifndef BOT_CLIENT_HPP
#define BOT_CLIENT_HPP

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <websocketpp/config.hpp>
#include <websocketpp/server.hpp>

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

struct QQMessage {
    std::string msg_type;
    int64_t user_id;
    int64_t group_id;
    std::string message;
    int64_t message_id;
};

class BotClient {
public:
    using EventHandler = std::function<void(const QQEvent&)>;

    BotClient();
    ~BotClient();

    bool connect(const std::string& ws_url, const std::string& access_token);
    void disconnect();

    bool sendMessage(const std::string& target_type, int64_t target_id, const std::string& message);

    bool sendGroupMessage(int64_t group_id, const std::string& message);
    bool sendPrivateMessage(int64_t user_id, const std::string& message);

    bool setGroupKick(int64_t group_id, int64_t user_id, bool reject_add_request = false);
    bool setGroupBan(int64_t group_id, int64_t user_id, int64_t duration);
    bool setGroupWholeBan(int64_t group_id, bool enable);
    bool setGroupCard(int64_t group_id, int64_t user_id, const std::string& card);
    bool setGroupTitle(int64_t group_id, int64_t user_id, const std::string& title);

    bool deleteMessage(int64_t message_id);

    void setMessageHandler(EventHandler handler);
    void setJoinHandler(EventHandler handler);

    bool isConnected() const { return connected_; }

private:
    void onMessage(websocketpp::connection_hdl hdl, websocketpp::server<>::message_ptr msg);
    void handleEvent(const QQEvent& event);

    void sendRequest(const std::string& action, const std::map<std::string, std::string>& params);

    bool connected_;
    std::string ws_url_;
    std::string access_token_;
    EventHandler message_handler_;
    EventHandler join_handler_;
    std::mutex mutex_;
    websocketpp::server<>* server_;
};

class EventDispatcher {
public:
    EventDispatcher();

    void registerMessageHandler(BotClient::EventHandler handler);
    void registerJoinHandler(BotClient::EventHandler handler);

    void dispatch(const QQEvent& event);

private:
    BotClient::EventHandler message_handler_;
    BotClient::EventHandler join_handler_;
};

}

#endif