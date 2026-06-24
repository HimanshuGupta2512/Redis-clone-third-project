#ifndef CACHELITE_PERSISTENCE_MANAGER_H
#define CACHELITE_PERSISTENCE_MANAGER_H

#include "kv_engine.h"

class PersistenceManager {
public:
    static void save_to_disk(KVEngine& engine);
    static void load_from_disk(KVEngine& engine);
};

#endif // CACHELITE_PERSISTENCE_MANAGER_H
