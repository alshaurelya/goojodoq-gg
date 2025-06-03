#include "file_utils.hpp"
#include <fstream>
#include <iostream>
#include <mutex>
#include <vector> // Untuk menyimpan array anomali dari JSON
#include "nlohmann/json.hpp" // Pastikan ini di-include

using json = nlohmann::json;

namespace { // Anonymous namespace
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
    json anomalies_json_array = json::array(); // Defaultnya array kosong

    // 1. Coba baca file JSON yang ada
    std::ifstream infile(json_filename);
    if (infile.is_open()) {
        try {
            // Hanya parse jika file tidak kosong
            if (infile.peek() != std::ifstream::traits_type::eof()) {
                 infile >> anomalies_json_array;
            }
            // Pastikan itu adalah array
            if (!anomalies_json_array.is_array()) {
                // Jika file ada tapi bukan array (misal file JSON lama yang error), timpa
                if (!anomalies_json_array.is_null()) { // is_null jika file kosong atau tidak valid
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

    // 2. Tambahkan anomali baru ke array
    json anomaly_obj = new_anomaly; // Menggunakan konversi otomatis dari NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE
    anomalies_json_array.push_back(anomaly_obj);

    // 3. Tulis kembali seluruh array ke file JSON
    std::ofstream outfile(json_filename, std::ios::trunc); // Timpa file lama
    if (!outfile) {
        std::cerr << "Error: Unable to open JSON file for writing: " << json_filename << std::endl;
        return;
    }
    outfile << anomalies_json_array.dump(4); // dump(4) untuk pretty print dengan indentasi 4 spasi
    if (!outfile) {
        std::cerr << "Error: Failed to write data to JSON file." << std::endl;
    }
    // outfile.close(); // Menutup otomatis
}