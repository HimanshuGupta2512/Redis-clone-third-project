#include "server.h"
#include "logger.h"
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

        // Phase 1: basic single-threaded echo and close
        char buffer[1024];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            
            // Trim trailing newlines for cleaner logging
            std::string msg(buffer);
            while (!msg.empty() && (msg.back() == '\r' || msg.back() == '\n')) {
                msg.pop_back();
            }
            
            Logger::log("Received: " + msg);
            send(client_socket, buffer, bytes_received, 0);
        } else if (bytes_received == 0) {
            Logger::log("Client disconnected during read");
        } else {
            Logger::error("recv failed");
        }

        closesocket(client_socket);
        Logger::log("Connection closed.");
    }

    Logger::log("Server shutdown complete.");
}
