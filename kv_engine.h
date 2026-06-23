#ifndef CACHELITE_KV_ENGINE_H
#define CACHELITE_KV_ENGINE_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <optional>

class KVEngine {
public:
    KVEngine() = default;

    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    bool del(const std::string& key);
    bool exists(const std::string& key);

private:
    std::unordered_map<std::string, std::string> store_;
    std::mutex mutex_;
};

#endif // CACHELITE_KV_ENGINE_H
