#ifndef CACHELITE_SERVER_H
#define CACHELITE_SERVER_H

#include "kv_engine.h"
#include <winsock2.h>
#include <atomic>

extern std::atomic<bool> running;
extern SOCKET listen_socket;

class Server {
public:
    explicit Server(KVEngine& engine);
    ~Server();

    void run();

private:
    KVEngine& engine_;
};

#endif // CACHELITE_SERVER_H
