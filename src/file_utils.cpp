#include "file_utils.hpp"
#include <fstream>
#include <iostream>
#include <mutex>
#include <vector>
#include <algorithm> // Untuk std::sort
#include <cmath>     // Untuk std::abs
#include "nlohmann/json.hpp"

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

void saveToJSON(const HeartbeatAnomaly& new_anomaly_struct) {
    std::lock_guard<std::mutex> lock(file_mutex);
    std::vector<HeartbeatAnomaly> anomalies_vec; // Gunakan vector dari struct untuk sorting

    // 1. Coba baca file JSON yang ada dan parse ke vector<HeartbeatAnomaly>
    std::ifstream infile(json_filename);
    if (infile.is_open()) {
        try {
            json temp_json_array;
            // Hanya parse jika file tidak kosong
            if (infile.peek() != std::ifstream::traits_type::eof()) {
                infile >> temp_json_array;
            }
            
            if (temp_json_array.is_array()) {
                for (const auto& item : temp_json_array) {
                        anomalies_vec.push_back(item.get<HeartbeatAnomaly>());
                }
            } else if (!temp_json_array.is_null()){ // Handle jika file ada tapi bukan array atau kosong
                 std::cerr << "Warning: Existing " << json_filename << " is not an array. Overwriting." << std::endl;
            }
        } catch (json::parse_error& e) {
            std::cerr << "Warning: Failed to parse existing " << json_filename << ": " << e.what() << ". Overwriting." << std::endl;
            anomalies_vec.clear(); // Kosongkan vector jika parsing gagal total
        } catch (json::type_error& e) { // Menangkap error jika get<HeartbeatAnomaly> gagal
            std::cerr << "Warning: Type error parsing anomaly from " << json_filename << ": " << e.what() << ". Clearing previous data." << std::endl;
            anomalies_vec.clear();
        }
        infile.close();
    }

    // 2. Tambahkan anomali baru ke vector
    anomalies_vec.push_back(new_anomaly_struct);

    // 3. Urutkan vector berdasarkan nilai absolut dari 'deviation' (selisih terbesar dulu)
    std::sort(anomalies_vec.begin(), anomalies_vec.end(), [](const HeartbeatAnomaly& a, const HeartbeatAnomaly& b) {
        return std::abs(a.deviation) > std::abs(b.deviation);
    });

    // 4. Konversi vector yang sudah diurutkan kembali ke json array
    json sorted_anomalies_json_array = anomalies_vec; // Menggunakan konversi otomatis

    // 5. Tulis kembali seluruh array yang sudah diurutkan ke file JSON
    std::ofstream outfile(json_filename, std::ios::trunc);
    if (!outfile) {
        std::cerr << "Error: Unable to open JSON file for writing: " << json_filename << std::endl;
        return;
    }
    outfile << sorted_anomalies_json_array.dump(4); // dump(4) untuk pretty print
    if (!outfile) {
        std::cerr << "Error: Failed to write data to JSON file." << std::endl;
    }
}