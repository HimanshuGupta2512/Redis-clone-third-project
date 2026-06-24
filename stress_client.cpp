#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <winsock2.h>
#include <ws2tcpip.h>

std::atomic<int> total_errors{0};

void trim_crlf(std::string& response) {
    while (!response.empty() && 
           (response.back() == '\r' || response.back() == '\n')) {
        response.pop_back();
    }
}

void stress_worker(int thread_id) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Thread " << thread_id << " failed to create socket.\n";
        total_errors++;
        return;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6379);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Thread " << thread_id << " failed to connect.\n";
        closesocket(sock);
        total_errors++;
        return;
    }

    int local_errors = 0;
    char buffer[1024];

    // 1000 SETs
    for (int i = 0; i < 1000; ++i) {
        std::string key = "key_" + std::to_string(thread_id) + "_" + std::to_string(i);
        std::string val = "val_" + std::to_string(thread_id) + "_" + std::to_string(i);
        std::string cmd = "SET " + key + " " + val + "\r\n";
        send(sock, cmd.c_str(), cmd.length(), 0);

        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::string response(buffer);
            trim_crlf(response);
            if (response != "OK") {
                local_errors++;
            }
        } else {
            local_errors++;
        }
    }

    // 1000 GETs
    for (int i = 0; i < 1000; ++i) {
        std::string key = "key_" + std::to_string(thread_id) + "_" + std::to_string(i);
        std::string expected_val = "val_" + std::to_string(thread_id) + "_" + std::to_string(i);
        std::string cmd = "GET " + key + "\r\n";
        send(sock, cmd.c_str(), cmd.length(), 0);

        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::string response(buffer);
            trim_crlf(response);
            if (response != expected_val) {
                local_errors++;
            }
        } else {
            local_errors++;
        }
    }

    closesocket(sock);
    total_errors += local_errors;
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    std::vector<std::thread> threads;
    
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(stress_worker, i);
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    double elapsed_seconds = elapsed.count();
    
    // Elapsed ms
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    
    int ops = 20000;
    double ops_sec = ops / elapsed_seconds;

    std::cout << "Total ops:    " << ops << "\n";
    std::cout << "Elapsed:      " << elapsed_ms << "ms\n";
    std::cout << "Throughput:   " << ops_sec << " ops/sec\n";
    
    if (total_errors > 0) {
        std::cout << "Correctness:  FAIL (" << total_errors << " errors)\n";
        WSACleanup();
        return 1;
    } else {
        std::cout << "Correctness:  PASS (0 errors)\n";
    }

    WSACleanup();
    return 0;
}
