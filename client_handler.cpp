#include "client_handler.h"
#include "command_parser.h"
#include "logger.h"
#include <string>
#include <vector>

ClientHandler::ClientHandler(KVEngine& engine) : engine_(engine) {}

void ClientHandler::handle_client(SOCKET client_socket) {
    char buffer[1024];
    std::string current_line = "";

    while (running) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            current_line += buffer;

            // Process full lines
            size_t pos;
            while ((pos = current_line.find('\n')) != std::string::npos) {
                std::string line = current_line.substr(0, pos);
                current_line.erase(0, pos + 1);

                // Trim trailing \r
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }

                if (line.empty()) continue;

                ParsedCommand cmd = CommandParser::parse(line);
                std::string response;

                if (!cmd.error.empty()) {
                    response = cmd.error + "\r\n";
                } else {
                    // Dummy implementations for Phase 2
                    if (cmd.name == "SET") {
                        if (cmd.args.size() == 2) response = "OK\r\n";
                        else response = "ERR wrong number of arguments for 'SET' command\r\n";
                    } else if (cmd.name == "GET") {
                        if (cmd.args.size() == 1) response = "(nil)\r\n";
                        else response = "ERR wrong number of arguments for 'GET' command\r\n";
                    } else if (cmd.name == "DEL") {
                        if (cmd.args.size() == 1) response = "0\r\n";
                        else response = "ERR wrong number of arguments for 'DEL' command\r\n";
                    } else if (cmd.name == "EXISTS") {
                        if (cmd.args.size() == 1) response = "0\r\n";
                        else response = "ERR wrong number of arguments for 'EXISTS' command\r\n";
                    } else if (cmd.name == "EXPIRE") {
                        if (cmd.args.size() == 2) response = "0\r\n";
                        else response = "ERR wrong number of arguments for 'EXPIRE' command\r\n";
                    } else {
                        response = "ERR unknown command '" + cmd.name + "'\r\n";
                    }
                }
                
                send(client_socket, response.c_str(), response.length(), 0);
            }
        } else if (bytes_received == 0) {
            Logger::log("Client disconnected gracefully.");
            break;
        } else {
            int error_code = WSAGetLastError();
            if (error_code == WSAETIMEDOUT) {
                // Expected timeout, check running flag and continue
                continue;
            } else {
                // Any other non-zero error code is a genuine error
                Logger::error("recv failed with error: " + std::to_string(error_code) + ". Terminating client thread.");
                break;
            }
        }
    }

    closesocket(client_socket);
}
