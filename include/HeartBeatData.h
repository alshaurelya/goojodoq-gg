#ifndef HEARTBEAT_DATA_H
#define HEARTBEAT_DATA_H

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Konstanta untuk deteksi anomali
const int BRADY_LIMIT = 60;
const int TACHY_LIMIT = 100;

struct HeartbeatRecord {
    long long timestamp; // Unix timestamp in millisecond
    int heartbeat_rate;
};

struct HeartbeatAnomaly {
    long long timestamp;
    int heartbeat_rate;
    int deviation;
};


inline void to_json(json& j, const HeartbeatAnomaly& ha) {
    j = json{
        {"timestamp", ha.timestamp},
        {"heartbeat_rate", ha.heartbeat_rate},
        {"deviation", ha.deviation}
    };
}

inline void from_json(const json& j, HeartbeatAnomaly& ha) {
    j.at("timestamp").get_to(ha.timestamp);
    j.at("heartbeat_rate").get_to(ha.heartbeat_rate);
    j.at("deviation").get_to(ha.deviation);
}

#endif