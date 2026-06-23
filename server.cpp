#include "server.h"
#include "client_handler.h"
#include "logger.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

Server::Server(KVEngine& engine) : engine_(engine) {
}

Server::~Server() {
}

void Server::run() {
    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == INVALID_SOCKET) {
        Logger::error("Failed to create socket");
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6379);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        Logger::error("Bind failed");
        closesocket(listen_socket);
        return;
    }

    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        Logger::error("Listen failed");
        closesocket(listen_socket);
        return;
    }

    Logger::log("Server listening on port 6379...");

    while (running) {
        sockaddr_in client_addr{};
        int client_len = sizeof(client_addr);
        SOCKET client_socket = accept(listen_socket, (sockaddr*)&client_addr, &client_len);

        if (client_socket == INVALID_SOCKET) {
            if (!running) {
                break; // Expected during shutdown
            }
            Logger::error("Accept failed");
            continue;
        }

        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, INET_ADDRSTRLEN);
        Logger::log("Client connected from " + std::string(ip_str));

        // Apply SO_RCVTIMEO to prevent recv hang during shutdown
        int timeout_ms = 1000;
        setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_ms, sizeof(timeout_ms));

        {
            std::lock_guard<std::mutex> lock(client_threads_mutex);
            client_threads.emplace_back([this, client_socket]() {
                ClientHandler handler(this->engine_);
                handler.handle_client(client_socket);
            });
        }
    }

    Logger::log("Server shutdown initiated. Joining client threads...");
    
    {
        std::lock_guard<std::mutex> lock(client_threads_mutex);
        for (auto& t : client_threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        client_threads.clear();
    }

    Logger::log("Server shutdown complete.");
}
