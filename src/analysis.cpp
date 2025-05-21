#include "analysis.hpp"

bool isAnomaly(const HeartbeatData& data, AnomalyRecord& outRecord) {
    if (data.heartRate < BRADY_LIMIT) {
        outRecord = {data.timestamp, data.heartRate, "bradycardia"};
        return true;
    } else if (data.heartRate > TACHY_LIMIT) {
        outRecord = {data.timestamp, data.heartRate, "tachycardia"};
        return true;
    }
    return false;
}
