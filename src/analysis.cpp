#include "analysis.hpp"
// #include <cmath> // Belum dipakai di versi error ini

bool isAnomaly(const HeartbeatRecord& data, HeartbeatAnomaly& outRecord) {
    if (data.heartbeat_rate < BRADY_LIMIT) {
        outRecord.timestamp = data.timestamp;
        outRecord.heartbeat_rate = data.heartbeat_rate;
        // KESALAHAN: Deviation untuk bradycardia seharusnya negatif
        outRecord.deviation = BRADY_LIMIT - data.heartbeat_rate; // Ini jadi positif
        return true;
    } else if (data.heartbeat_rate > TACHY_LIMIT) {
        outRecord.timestamp = data.timestamp;
        outRecord.heartbeat_rate = data.heartbeat_rate;
        outRecord.deviation = data.heartbeat_rate - TACHY_LIMIT; // Ini positif, benar untuk tachy
        return true;
    }
    outRecord.deviation = 0; // Default jika tidak anomali
    return false;
}