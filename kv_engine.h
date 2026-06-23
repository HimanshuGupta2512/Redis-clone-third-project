#ifndef CACHELITE_KV_ENGINE_H
#define CACHELITE_KV_ENGINE_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <optional>
#include <chrono>

class KVEngine {
public:
    KVEngine() = default;

    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    bool del(const std::string& key);
    bool exists(const std::string& key);
    bool expire(const std::string& key, int seconds);
    void sweep_expired_keys();

private:
    std::unordered_map<std::string, std::string> store_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> ttl_map_;
    std::mutex mutex_;
};

#endif // CACHELITE_KV_ENGINE_H
