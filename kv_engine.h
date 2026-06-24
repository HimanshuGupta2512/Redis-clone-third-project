#ifndef CACHELITE_KV_ENGINE_H
#define CACHELITE_KV_ENGINE_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <optional>
#include <chrono>
#include <vector>
#include <tuple>

class KVEngine {
public:
    KVEngine() = default;

    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    bool del(const std::string& key);
    bool exists(const std::string& key);
    bool expire(const std::string& key, int seconds);
    void sweep_expired_keys();

    std::vector<std::tuple<std::string, std::string, std::optional<std::chrono::steady_clock::time_point>>> get_all_for_persistence();
    void load_from_persistence(const std::string& key, const std::string& value, std::optional<std::chrono::steady_clock::time_point> ttl);

private:
    std::unordered_map<std::string, std::string> store_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> ttl_map_;
    std::mutex mutex_;
};

#endif // CACHELITE_KV_ENGINE_H
