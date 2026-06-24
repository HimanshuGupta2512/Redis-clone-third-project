#include "logger.h"
#include <chrono>
#include <iomanip>

std::mutex Logger::log_mutex;

static std::string get_current_time_str() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::string time_str;
    char buffer[100];
#ifdef _WIN32
    struct tm timeinfo;
    localtime_s(&timeinfo, &in_time_t);
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
#else
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&in_time_t));
#endif
    return std::string(buffer);
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::cout << "[" << get_current_time_str() << "] [INFO] " << message << std::endl;
}

void Logger::error(const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::cerr << "[" << get_current_time_str() << "] [ERROR] " << message << std::endl;
}
