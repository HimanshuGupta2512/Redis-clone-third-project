#include "server.h"
#include "kv_engine.h"
#include "logger.h"
#include "ttl_manager.h"
#include "persistence_manager.h"
#include <winsock2.h>
#include <windows.h>
#include <thread>

std::atomic<bool> running{true};
SOCKET listen_socket = INVALID_SOCKET;

BOOL WINAPI console_ctrl_handler(DWORD ctrl_type) {
    if (ctrl_type == CTRL_C_EVENT || ctrl_type == CTRL_BREAK_EVENT) {
        Logger::log("Shutdown signal received.");
        running = false;
        if (listen_socket != INVALID_SOCKET) {
            closesocket(listen_socket);
        }
        return TRUE;
    }
    return FALSE;
}

int main() {
    SetConsoleCtrlHandler(console_ctrl_handler, TRUE);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logger::error("WSAStartup failed");
        return 1;
    }

    KVEngine engine; // Single global instance

    PersistenceManager::load_from_disk(engine);

    TTLManager ttl_manager(engine);
    std::thread ttl_thread(&TTLManager::run, &ttl_manager);

    Server server(engine);
    server.run(); // Blocks until running is false. Client threads joined inside run().

    Logger::log("Joining TTL Manager thread...");
    if (ttl_thread.joinable()) {
        ttl_thread.join();
    }

    Logger::log("Saving to disk...");
    PersistenceManager::save_to_disk(engine);

    // Normal shutdown sequence
    WSACleanup();
    return 0;
}
