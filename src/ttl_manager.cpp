#include "ttl_manager.h"
#include <thread>
#include <chrono>

TTLManager::TTLManager(KVEngine& engine) : engine_(engine) {}

void TTLManager::run() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (running) {
            engine_.sweep_expired_keys();
        }
    }
}
