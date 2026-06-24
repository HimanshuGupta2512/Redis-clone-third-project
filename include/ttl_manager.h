#ifndef CACHELITE_TTL_MANAGER_H
#define CACHELITE_TTL_MANAGER_H

#include "kv_engine.h"
#include <atomic>

extern std::atomic<bool> running;

class TTLManager {
public:
    explicit TTLManager(KVEngine& engine);
    void run();

private:
    KVEngine& engine_;
};

#endif // CACHELITE_TTL_MANAGER_H
