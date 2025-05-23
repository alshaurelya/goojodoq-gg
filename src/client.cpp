#include <iostream>
#include <winsock2.h>
#include <ctime>
#include <thread>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr = {AF_INET, htons(8888)};
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    std::cout << "Connected to server.\n";

    while (true) {
        time_t now = time(nullptr);
        int hr = 50 + rand() % 71;
        std::string msg = std::to_string(now) + ";" + std::to_string(hr);
        send(sock, msg.c_str(), msg.size(), 0);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}