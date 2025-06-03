#include <iostream>
#include <winsock2.h>
#include <string>
#include <chrono> // Untuk timestamp milidetik
#include <thread>
// #include <vector> // Tidak digunakan secara langsung di client ini
#include <cstdlib> // Untuk rand, srand
#include <ctime>   // Untuk time

#pragma comment(lib, "ws2_32.lib")

// using namespace std; // Sebaiknya dihindari di global scope
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::this_thread::sleep_for;


int main() {
    srand(time(0)); // Seed random number generator

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cerr << "Failed. Error Code : " << WSAGetLastError() << endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Could not create socket : " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Connect failed with error code : " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    cout << "Connected to server." << endl;

    while (true) {
        long long now_ms = duration_cast<milliseconds>(
                              system_clock::now().time_since_epoch()
                          ).count();
        int hr = 50 + rand() % 71; // Simulasi denyut jantung antara 50 dan 120

        string msg = to_string(now_ms) + "+" + to_string(hr);

        if (send(sock, msg.c_str(), msg.length(), 0) < 0) {
            cerr << "Send failed" << endl;
            break;
        }
        // Debug: cout << "Sent: " << msg << endl;

        sleep_for(seconds(1));
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}