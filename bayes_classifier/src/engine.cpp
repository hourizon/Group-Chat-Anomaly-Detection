#include "engine.hpp"

DetectorEngine::DetectorEngine() {
    // 在构造时就加载训练好的模型
    try {
        bayes::Trainer trainer;
        trainer.train("data/ham.txt", "ham");
        trainer.train("data/spam.txt", "spam");
        classifier_ = trainer.getClassifier();
        std::cout << "[Engine] 朴素贝叶斯模型加载成功。" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Engine] 模型加载失败: " << e.what() << std::endl;
        throw;
    }
}

QQEvent DetectorEngine::parse_event(const std::string& json_str) {
    QQEvent event;
    try {
        auto json = nlohmann::json::parse(json_str);
        event.post_type = json.value("post_type", "");

        if (event.post_type == "message") {
            event.message_type = json.value("message_type", "");
            event.user_id = json.value("user_id", 0);
            event.message = json.value("message", "");
            event.message_id = json.value("message_id", 0);
            if (event.message_type == "group") {
                event.group_id = json.value("group_id", 0);
            }
        } else if (event.post_type == "request" && json.value("request_type", "") == "group") {
            event.request_type = "group";
            event.sub_type = json.value("sub_type", ""); // "add" or "invite"
            event.user_id = json.value("user_id", 0);
            event.group_id = json.value("group_id", 0);
        }
        // 注意：go-cqhttp的群成员增加是 notice 事件，这里简化处理
        // 真实的群成员增加事件类型是 notice, notice_type 是 group_increase
        else if (event.post_type == "notice" && json.value("notice_type", "") == "group_increase") {
             event.post_type = "group_join"; // 我们将其统一为自己的事件类型
             event.user_id = json.value("user_id", 0);
             event.group_id = json.value("group_id", 0);
        }

    } catch (const nlohmann::json::parse_error& e) {
        // 忽略心跳包等非标准JSON事件
    }
    return event;
}

void DetectorEngine::process_raw_event(const std::string& json_str) {
    QQEvent event = parse_event(json_str);

    // --- 事件分发 ---
    if (event.post_type == "message" && event.message_type == "group") {
        double spam_prob = classifier_.classify(event.message);
        std::string category = (spam_prob > 0.9) ? "spam" : "ham"; // 阈值设为0.9
        std::cout << "[Content] " << event.group_id << "/" << event.user_id << ": " << event.message << " -> " << category << " (P=" << spam_prob << ")" << std::endl;
        if (category == "spam") {
            handle_spam_message(event);
        }
    }
    else if (event.post_type == "group_join") {
        profiler_.add_event(event.user_id, "GROUP_JOIN");
        std::cout << "[Behavior] " << event.user_id << " 加入了群 " << event.group_id << std::endl;
        if (profiler_.is_abnormal(event.user_id, "GROUP_JOIN")) {
            handle_abnormal_join(event);
        }
    }
}

void DetectorEngine::handle_spam_message(const QQEvent& event) {
    std::cout << "[ACTION] 检测到垃圾消息! 准备删除消息 " << event.message_id << " 并禁言 " << event.user_id << " 60秒。" << std::endl;
    // 此处将是调用 go-cqhttp API 的代码
    // 例如: http_client.post("/delete_msg", ...);
    // http_client.post("/set_group_ban", ...);
}

void DetectorEngine::handle_abnormal_join(const QQEvent& event) {
    std::cout << "[ACTION] 检测到异常加群行为! 准备将用户 " << event.user_id << " 踢出群 " << event.group_id << std::endl;
    // 此处将是调用 go-cqhttp API 的代码
    // 例如: http_client.post("/set_group_kick", ...);
}
