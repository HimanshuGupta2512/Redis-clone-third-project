#include "kv_engine.h"

void KVEngine::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    store_[key] = value;
}

std::optional<std::string> KVEngine::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = store_.find(key);
    if (it != store_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool KVEngine::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    return store_.erase(key) > 0;
}

bool KVEngine::exists(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    return store_.find(key) != store_.end();
}
