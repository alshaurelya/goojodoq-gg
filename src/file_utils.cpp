#include "file_utils.hpp"
#include <fstream>
#include <mutex>
#include <iostream>
#include <json/json.h>

namespace {
    std::mutex file_mutex;
    constexpr const char* binary_filename = "data_log.bin";
    constexpr const char* json_filename = "anomaly_log.json";
}

void saveToBinary(const HeartbeatData& data) {
    std::lock_guard<std::mutex> lock(file_mutex);
    std::ofstream out(binary_filename, std::ios::binary | std::ios::app);
    if (!out) {
        std::cerr << "Error: Unable to open binary file for writing.\n";
        return;
    }

    out.write(reinterpret_cast<const char*>(&data), sizeof(data));
    if (!out) {
        std::cerr << "Error: Failed to write data to binary file.\n";
    }
}

void saveToJSON(const AnomalyRecord& record) {
    std::lock_guard<std::mutex> lock(file_mutex);

    Json::Value entry;
    entry["timestamp"] = static_cast<Json::Int64>(record.timestamp);
    entry["heartRate"] = record.heartRate;
    entry["status"] = record.status;

    Json::Value logArray;

    // Read existing JSON entries if file exists
    std::ifstream inFile(json_filename);
    if (inFile) {
        try {
            inFile >> logArray;
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to parse existing JSON file. Overwriting. (" << e.what() << ")\n";
            logArray = Json::Value(Json::arrayValue);
        }
    }

    logArray.append(entry);

    std::ofstream outFile(json_filename, std::ios::trunc);
    if (!outFile) {
        std::cerr << "Error: Unable to open JSON file for writing.\n";
        return;
    }

    outFile << logArray;
    if (!outFile) {
        std::cerr << "Error: Failed to write data to JSON file.\n";
    }
}
