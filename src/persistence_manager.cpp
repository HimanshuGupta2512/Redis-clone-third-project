#include "persistence_manager.h"
#include "logger.h"
#include <fstream>
#include <sstream>

constexpr long long NO_TTL = -1;

void PersistenceManager::save_to_disk(KVEngine& engine) {
    auto snapshot = engine.get_all_for_persistence();
    std::ofstream out("dump.cache", std::ios::trunc);
    if (!out) {
        Logger::error("Failed to open dump.cache for writing");
        return;
    }

    for (const auto& [key, value, ttl] : snapshot) {
        long long ms = NO_TTL;
        if (ttl.has_value()) {
            auto expiry_point = ttl.value();
            auto remaining = expiry_point - std::chrono::steady_clock::now();
            auto wall_expiry = std::chrono::system_clock::now() + remaining;
            ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                wall_expiry.time_since_epoch()).count();
        }
        out << key << " " << value << " " << ms << "\n";
    }
    Logger::log("Persistence: saved " + std::to_string(snapshot.size()) + " keys to dump.cache");
}

void PersistenceManager::load_from_disk(KVEngine& engine) {
    std::ifstream in("dump.cache");
    if (!in) {
        Logger::log("Persistence: no dump.cache found, starting fresh");
        return;
    }

    std::string line;
    int loaded = 0;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string key, value;
        long long ms;
        if (iss >> key >> value >> ms) {
            std::optional<std::chrono::steady_clock::time_point> ttl_opt;
            if (ms != NO_TTL) {
                auto wall_expiry = std::chrono::system_clock::time_point(
                    std::chrono::milliseconds(ms));
                auto now_system = std::chrono::system_clock::now();
                if (wall_expiry <= now_system) continue;
                auto remaining = wall_expiry - now_system;
                auto steady_expiry = std::chrono::steady_clock::now() + remaining;
                ttl_opt = steady_expiry;
            }
            engine.load_from_persistence(key, value, ttl_opt);
            loaded++;
        }
    }
    Logger::log("Persistence: loaded " + std::to_string(loaded) + " keys from dump.cache");
}
