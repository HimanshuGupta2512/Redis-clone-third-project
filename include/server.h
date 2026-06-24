#ifndef CACHELITE_SERVER_H
#define CACHELITE_SERVER_H

#include "kv_engine.h"
#include <winsock2.h>
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>

extern std::atomic<bool> running;
extern SOCKET listen_socket;

class Server {
public:
    explicit Server(KVEngine& engine);
    ~Server();

    void run();

private:
    KVEngine& engine_;

    // Client threads are joined en masse during shutdown.
    // Threads are never detached. Finished threads remain
    // in this vector until shutdown — this is intentional.
    std::vector<std::thread> client_threads;
    std::mutex client_threads_mutex;
};

#endif // CACHELITE_SERVER_H
