#ifndef CACHELITE_CLIENT_HANDLER_H
#define CACHELITE_CLIENT_HANDLER_H

#include <winsock2.h>
#include "kv_engine.h"
#include <atomic>

extern std::atomic<bool> running;

class ClientHandler {
public:
    explicit ClientHandler(KVEngine& engine);
    void handle_client(SOCKET client_socket);

private:
    KVEngine& engine_;
};

#endif // CACHELITE_CLIENT_HANDLER_H
