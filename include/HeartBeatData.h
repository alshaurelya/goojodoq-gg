#ifndef HEARTBEAT_DATA_H
#define HEARTBEAT_DATA_H

#include <string>
#include <vector>
#include <cstring> 
#include "nlohmann/json.hpp"

// Konstanta untuk deteksi anomali
const int BRADY_LIMIT = 60;
const int TACHY_LIMIT = 100;

struct HeartbeatRecord {
    long long timestamp;
    int heartbeat_rate;
};

struct HeartbeatAnomaly {
    long long timestamp;
    int heartbeat_rate;
    int deviation;

    // Untuk serialisasi/deserialisasi JSON dengan nlohmann
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HeartbeatAnomaly, timestamp, heartbeat_rate, deviation)
};

#endif