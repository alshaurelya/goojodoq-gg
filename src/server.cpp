#include <iostream>
#include <winsock2.h>
#include <thread>
#include <sstream>
#include "data.hpp"
#include "file_utils.hpp"
#include "analysis.hpp"

#pragma comment(lib, "ws2_32.lib")

void handleClient(SOCKET clientSocket) {
    char buffer[128];
    while (true) {
        int len = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) break;
        buffer[len] = '\0';

        std::istringstream ss(buffer);
        time_t t; int hr;
        char delim;
        ss >> t >> delim >> hr;

        HeartbeatData data{t, hr};
        saveToBinary(data);

        AnomalyRecord rec;
        if (isAnomaly(data, rec)) {
            saveToJSON(rec);
        }
    }
    closesocket(clientSocket);
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr = {AF_INET, htons(8888)};
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);
    std::cout << "Server listening on port 8888...\n";

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        std::thread(handleClient, clientSocket).detach();
    }

    WSACleanup();
    return 0;
}