// #define CPPHTTPLIB_OPENSSL_SUPPORT // 如果需要HTTPS支持，可以打开此宏
#include "httplib.h"
#include "engine.hpp"
#include <csignal> // 用于处理 Ctrl+C
#include <memory>

// 全局智能指针，用于优雅地停止服务器
std::unique_ptr<httplib::Server> svr;

// Ctrl+C 信号处理函数
void signal_handler(int signum) {
    std::cout << "\n捕获到中断信号，正在优雅地关闭服务器..." << std::endl;
    if (svr) {
        svr->stop();
    }
    exit(signum);
}

int main() {
    // 注册信号处理器
    signal(SIGINT, signal_handler);

    try {
        // --- 1. 初始化检测引擎 ---
        DetectorEngine engine;

        // --- 2. 设置HTTP服务器 ---
        svr = std::make_unique<httplib::Server>();

        // 设置 go-cqhttp 事件上报的端点 (通常是根目录 "/")
        svr->Post("/", [&](const httplib::Request& req, httplib::Response& res) {
            // 将接收到的原始JSON事件交给引擎处理
            engine.process_raw_event(req.body);

            // 立即响应 go-cqhttp，表示我们已收到，防止超时
            res.set_content("OK", "text/plain; charset=utf-8");
        });

        // --- 3. 启动服务器 ---
        std::cout << "[Server] C++ 检测引擎服务器已启动。" << std::endl;
        std::cout << "[Server] 正在监听 http://0.0.0.0:8081 ..." << std::endl;
        std::cout << "[Server] 请配置您的 go-cqhttp, 将事件上报到此地址。" << std::endl;
        std::cout << "[Server] 按 Ctrl+C 关闭服务器。" << std::endl;
        
        svr->listen("0.0.0.0", 8081);

    } catch (const std::exception& e) {
        std::cerr << "程序启动时发生严重错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
