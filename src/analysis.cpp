#include "analysis.hpp"

bool isAnomaly(const HeartbeatRecord& data, HeartbeatAnomaly& outRecord) {
    if (data.heartbeat_rate < BRADY_LIMIT) {
        outRecord.timestamp = data.timestamp;
        outRecord.heartbeat_rate = data.heartbeat_rate;
        outRecord.deviation = data.heartbeat_rate - BRADY_LIMIT;
        return true;
    } else if (data.heartbeat_rate > TACHY_LIMIT) {
        outRecord.timestamp = data.timestamp;
        outRecord.heartbeat_rate = data.heartbeat_rate;
        outRecord.deviation = data.heartbeat_rate - TACHY_LIMIT;
        return true;
    }
    outRecord.deviation = 0;
    return false;
}