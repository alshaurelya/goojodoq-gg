#include "file_utils.hpp"
#include <fstream>
#include <mutex>
#include <json/json.h>

std::mutex file_mutex;

void saveToBinary(const HeartbeatData& data) {
    std::lock_guard<std::mutex> lock(file_mutex);
    std::ofstream out("data_log.bin", std::ios::binary | std::ios::app);
    out.write(reinterpret_cast<const char*>(&data), sizeof(data));
}

void saveToJSON(const AnomalyRecord& record) {
    std::lock_guard<std::mutex> lock(file_mutex);
    Json::Value entry;
    entry["timestamp"] = static_cast<Json::Int64>(record.timestamp);
    entry["heartRate"] = record.heartRate;
    entry["status"] = record.status;

    std::ifstream inFile("anomaly_log.json");
    Json::Value logArray;
    if (inFile) inFile >> logArray;

    logArray.append(entry);

    std::ofstream outFile("anomaly_log.json");
    outFile << logArray;
}
