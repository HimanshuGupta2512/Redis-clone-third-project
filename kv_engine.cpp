#include "kv_engine.h"
#include "logger.h"

void KVEngine::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    store_[key] = value;
    ttl_map_.erase(key); // SET removes any existing TTL
}

std::optional<std::string> KVEngine::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Lazy expiration check
    auto ttl_it = ttl_map_.find(key);
    if (ttl_it != ttl_map_.end() && ttl_it->second <= std::chrono::steady_clock::now()) {
        store_.erase(key);
        ttl_map_.erase(ttl_it);
        return std::nullopt;
    }

    auto it = store_.find(key);
    if (it != store_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool KVEngine::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    ttl_map_.erase(key);
    return store_.erase(key) > 0;
}

bool KVEngine::exists(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Lazy expiration check
    auto ttl_it = ttl_map_.find(key);
    if (ttl_it != ttl_map_.end() && ttl_it->second <= std::chrono::steady_clock::now()) {
        store_.erase(key);
        ttl_map_.erase(ttl_it);
        return false;
    }

    return store_.find(key) != store_.end();
}

bool KVEngine::expire(const std::string& key, int seconds) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Lazy expiration check first
    auto ttl_it = ttl_map_.find(key);
    if (ttl_it != ttl_map_.end() && ttl_it->second <= std::chrono::steady_clock::now()) {
        store_.erase(key);
        ttl_map_.erase(ttl_it);
        return false;
    }

    if (store_.find(key) == store_.end()) {
        return false;
    }

    ttl_map_[key] = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    return true;
}

void KVEngine::sweep_expired_keys() {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = ttl_map_.begin();
    int count = 0;
    std::string purged_keys = "";

    while (it != ttl_map_.end()) {
        if (it->second <= std::chrono::steady_clock::now()) {
            store_.erase(it->first);
            if (count > 0) purged_keys += ", ";
            purged_keys += it->first;
            count++;
            it = ttl_map_.erase(it);
        } else {
            ++it;
        }
    }

    if (count > 0) {
        Logger::log("Sweep: purged " + std::to_string(count) + " expired key(s): " + purged_keys);
    }
}

std::vector<std::tuple<std::string, std::string, std::optional<std::chrono::steady_clock::time_point>>> KVEngine::get_all_for_persistence() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::tuple<std::string, std::string, std::optional<std::chrono::steady_clock::time_point>>> snapshot;
    for (const auto& [k, v] : store_) {
        std::optional<std::chrono::steady_clock::time_point> ttl;
        if (ttl_map_.find(k) != ttl_map_.end()) {
            ttl = ttl_map_[k];
        }
        snapshot.emplace_back(k, v, ttl);
    }
    return snapshot;
}

void KVEngine::load_from_persistence(const std::string& key, const std::string& value, std::optional<std::chrono::steady_clock::time_point> ttl) {
    std::lock_guard<std::mutex> lock(mutex_);
    store_[key] = value;
    if (ttl.has_value()) {
        ttl_map_[key] = ttl.value();
    } else {
        ttl_map_.erase(key);
    }
}
