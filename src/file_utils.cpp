#include "file_utils.hpp"
#include <fstream>
#include <iostream>
#include <mutex>
#include <vector> // Untuk menyimpan array anomali dari JSON
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace {
    std::mutex file_mutex;
    const char* binary_filename = "data_log.bin";
    const char* json_filename = "anomaly_log.json";
}

void saveToBinary(const HeartbeatRecord& data) {
    std::lock_guard<std::mutex> lock(file_mutex);
    std::ofstream out(binary_filename, std::ios::binary | std::ios::app);
    if (!out) {
        std::cerr << "Error: Unable to open binary file for writing: " << binary_filename << std::endl;
        return;
    }
    out.write(reinterpret_cast<const char*>(&data), sizeof(HeartbeatRecord));
    if (!out) {
        std::cerr << "Error: Failed to write data to binary file." << std::endl;
    }
}

void saveToJSON(const HeartbeatAnomaly& new_anomaly) {
    std::lock_guard<std::mutex> lock(file_mutex);
    json anomalies_json_array = json::array(); // default array kosong

    // membaca file JSON yang ada
    std::ifstream infile(json_filename);
    if (infile.is_open()) {
        try {
            if (infile.peek() != std::ifstream::traits_type::eof()) {
                 infile >> anomalies_json_array;
            }
            if (!anomalies_json_array.is_array()) {
                if (!anomalies_json_array.is_null()) {
                     std::cerr << "Warning: Existing " << json_filename << " is not an array. Overwriting." << std::endl;
                }
                anomalies_json_array = json::array();
            }
        } catch (json::parse_error& e) {
            std::cerr << "Warning: Failed to parse existing " << json_filename << ": " << e.what() << ". Overwriting." << std::endl;
            anomalies_json_array = json::array(); // Reset jika parsing gagal
        }
        infile.close();
    }

    // menambah anomali baru ke array
    json anomaly_obj = new_anomaly;
    anomalies_json_array.push_back(anomaly_obj);

    // menulis kembali seluruh array ke file JSON
    std::ofstream outfile(json_filename, std::ios::trunc); // menimpa file lama
    if (!outfile) {
        std::cerr << "Error: Unable to open JSON file for writing: " << json_filename << std::endl;
        return;
    }
    outfile << anomalies_json_array.dump(4);
    if (!outfile) {
        std::cerr << "Error: Failed to write data to JSON file." << std::endl;
    }
}