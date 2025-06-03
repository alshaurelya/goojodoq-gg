#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h> // For inet_ntop
#include <string>
#include <vector>
#include <thread>
#include <sstream>      // Untuk istringstream
#include "HeartBeatData.h"
#include "file_utils.hpp"
#include "analysis.hpp"

#pragma comment(lib, "ws2_32.lib")

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::thread;
using std::istringstream; // Digunakan untuk parsing


void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived;

    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        string received_str(buffer);
        // cout << "Server received: " << received_str << endl; // Debug

        HeartbeatRecord current_data;
        HeartbeatAnomaly anomaly_record;

        istringstream iss(received_str);
        long long ts_val;
        int hr_val;
        char delimiter;

        if (iss >> ts_val >> delimiter >> hr_val && delimiter == '+') {
            current_data.timestamp = ts_val;
            current_data.heartbeat_rate = hr_val;

            saveToBinary(current_data); // Simpan semua data mentah

            if (isAnomaly(current_data, anomaly_record)) {
                cout << "Anomaly detected: HR=" << anomaly_record.heartbeat_rate
                        << ", Deviation=" << anomaly_record.deviation
                        << ", Timestamp=" << anomaly_record.timestamp << endl;
                saveToJSON(anomaly_record); // saveToJSON akan menangani pengurutan internal
            }
        } else {
            cerr << "Invalid data format from client: " << received_str << endl;
        }
    }

    if (bytesReceived == 0) {
        cout << "Client disconnected." << endl;
    } else if (bytesReceived == SOCKET_ERROR) {
        cerr << "recv failed with error " << WSAGetLastError() << endl;
    }
    closesocket(clientSocket);
    // cout << "Socket closed for a client." << endl;
}

// main function server (sama seperti di Push 6)
int main() {
    WSADATA wsa;
    cout << "Initialising Winsock..." << endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cerr << "Failed. Error Code : " << WSAGetLastError() << endl;
        return 1;
    }
    cout << "Initialised." << endl;

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Could not create socket : " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }
    cout << "Socket created." << endl;

    sockaddr_in serverAddr_s;
    serverAddr_s.sin_family = AF_INET;
    serverAddr_s.sin_addr.s_addr = INADDR_ANY;
    serverAddr_s.sin_port = htons(8888);

    if (bind(serverSocket, (sockaddr*)&serverAddr_s, sizeof(serverAddr_s)) == SOCKET_ERROR) {
        cerr << "Bind failed with error code : " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    cout << "Bind done." << endl;

    listen(serverSocket, 5);

    cout << "Server listening on port 8888..." << endl;
    cout << "Waiting for incoming connections..." << endl;

    SOCKET clientSocket_temp;
    sockaddr_in clientAddr_s;
    int clientAddrSize = sizeof(clientAddr_s);

    vector<thread> client_threads;

    while ((clientSocket_temp = accept(serverSocket, (sockaddr*)&clientAddr_s, &clientAddrSize)) != INVALID_SOCKET) {
        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr_s.sin_addr, clientIp, INET_ADDRSTRLEN);
        
        cout << "Connection accepted from " << clientIp << ":" << ntohs(clientAddr_s.sin_port) << endl;
        
        client_threads.emplace_back(handleClient, clientSocket_temp);
        client_threads.back().detach();
    }

    if (clientSocket_temp == INVALID_SOCKET) {
        cerr << "accept failed with error code : " << WSAGetLastError() << endl;
    }
    
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}