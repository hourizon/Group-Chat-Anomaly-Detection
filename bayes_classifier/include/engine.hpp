#pragma once

#include <iostream>
#include <string>
#include "bayes.hpp"
#include "profiler.hpp"
#include "nlohmann/json.hpp"

// 定义了从 go-cqhttp 解析出的、我们关心的事件结构
struct QQEvent {
    std::string post_type;
    std::string message_type;
    std::string request_type;
    std::string sub_type;
    long long user_id = 0;
    long long group_id = 0;
    std::string message;
    long long message_id = 0;
};

// 负责调度所有模块的引擎
class DetectorEngine {
public:
    DetectorEngine();

    // 核心入口：处理原始的JSON事件字符串
    void process_raw_event(const std::string& json_str);

private:
    // 解析JSON字符串为结构化的QQEvent
    QQEvent parse_event(const std::string& json_str);

    // 我们的算法模块实例
    bayes::Classifier classifier_;
    profiler::BehaviorProfiler profiler_;

    // 处置函数
    void handle_spam_message(const QQEvent& event);
    void handle_abnormal_join(const QQEvent& event);
};
