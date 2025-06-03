#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h> // Untuk inet_ntop (lebih aman dari inet_ntoa)
#include <string>
#include <vector>
#include <thread>
#include <sstream>      // Untuk istringstream (belum dipakai di versi error ini)
#include "HeartBeatData.h" // Struktur data baru
#include "file_utils.hpp" // Utilitas file baru
#include "analysis.hpp"   // Analisis baru

#pragma comment(lib, "ws2_32.lib")

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::thread;
using std::stoll; // C++11
using std::stoi;  // C++11

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived;

    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        string received_str(buffer);
        // cout << "Server received: " << received_str << endl; // Debug

        HeartbeatRecord current_data;
        HeartbeatAnomaly anomaly_record;

        // Parsing data (timestamp_ms+heart_rate)
        // KESALAHAN PARSING SENGAJA: Misal salah pakai find atau substr
        size_t delimiter_pos = received_str.find('+');
        if (delimiter_pos != string::npos) {
            string ts_str = received_str.substr(0, delimiter_pos);
            // SENGAJA SALAH: Ambil substring yang salah untuk HR
            string hr_str = received_str.substr(delimiter_pos + 2); // Seharusnya delimiter_pos + 1

            try {
                current_data.timestamp = stoll(ts_str);
                current_data.heartbeat_rate = stoi(hr_str); // Akan error jika hr_str salah

                // Simpan semua data mentah ke biner
                saveToBinary(current_data);

                if (isAnomaly(current_data, anomaly_record)) {
                    cout << "Anomaly detected: HR=" << anomaly_record.heartbeat_rate
                         << ", Deviation=" << anomaly_record.deviation << endl;
                    saveToJSON(anomaly_record);
                }
            } catch (const std::invalid_argument& ia) {
                cerr << "Invalid argument for conversion: " << ia.what() << " for string: " << received_str << endl;
            } catch (const std::out_of_range& oor) {
                cerr << "Out of range for conversion: " << oor.what() << " for string: " << received_str << endl;
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
    // cout << "Socket closed for a client." << endl; // Bisa di-uncomment untuk debugging
}

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

    sockaddr_in serverAddr_s; // Penamaan variabel server address
    serverAddr_s.sin_family = AF_INET;
    serverAddr_s.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces
    serverAddr_s.sin_port = htons(8888);

    if (bind(serverSocket, (sockaddr*)&serverAddr_s, sizeof(serverAddr_s)) == SOCKET_ERROR) {
        cerr << "Bind failed with error code : " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    cout << "Bind done." << endl;

    listen(serverSocket, 5); // Max 5 pending connections

    cout << "Server listening on port 8888..." << endl;
    cout << "Waiting for incoming connections..." << endl;

    SOCKET clientSocket_temp; // Variabel temporary untuk accepted socket
    sockaddr_in clientAddr_s; // Variabel untuk info client address
    int clientAddrSize = sizeof(clientAddr_s);

    vector<thread> client_threads;

    while ((clientSocket_temp = accept(serverSocket, (sockaddr*)&clientAddr_s, &clientAddrSize)) != INVALID_SOCKET) {
        char clientIp[INET_ADDRSTRLEN]; // Buffer untuk IP address string
        inet_ntop(AF_INET, &clientAddr_s.sin_addr, clientIp, INET_ADDRSTRLEN); // Konversi IP ke string
        
        cout << "Connection accepted from " << clientIp << ":" << ntohs(clientAddr_s.sin_port) << endl;
        
        client_threads.emplace_back(handleClient, clientSocket_temp);
        client_threads.back().detach(); // Detach thread agar server bisa terima koneksi lain
    }

    if (clientSocket_temp == INVALID_SOCKET) {
        cerr << "accept failed with error code : " << WSAGetLastError() << endl;
    }
    
    // Cleanup (Idealnya, join threads sebelum cleanup jika tidak di-detach)
    // for (auto& th : client_threads) {
    //     if (th.joinable()) {
    //         th.join();
    //     }
    // }
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}