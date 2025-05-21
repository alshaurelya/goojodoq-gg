#pragma once
#include <ctime>
#include <string>

struct HeartbeatData {
    time_t timestamp;
    int heartRate;
};

struct AnomalyRecord {
    time_t timestamp;
    int heartRate;
    std::string status; // "bradycardia" or "tachycardia"
};

const int BRADY_LIMIT = 60;
const int TACHY_LIMIT = 100;