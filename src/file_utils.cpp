#include "file_utils.hpp"
#include <fstream>
#include <iostream>
#include <mutex>
#include <vector>
#include <algorithm>
#include <cmath>


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

void saveToJSON(const HeartbeatAnomaly& new_anomaly_struct) {
    std::lock_guard<std::mutex> lock(file_mutex);
    std::vector<HeartbeatAnomaly> anomalies_vec;
    nlohmann::json temp_json_array_from_file = nlohmann::json::array();

    std::ifstream infile(json_filename);
    if (infile.is_open()) {
        try {
            if (infile.peek() != std::ifstream::traits_type::eof()) {
                infile >> temp_json_array_from_file;
            }
            
            if (temp_json_array_from_file.is_array()) {
                for (const auto& item_json : temp_json_array_from_file) {
                    anomalies_vec.push_back(item_json.get<HeartbeatAnomaly>());
                }
            } else if (!temp_json_array_from_file.is_null()){
                 std::cerr << "Warning: Existing " << json_filename << " is not an array or is invalid. Overwriting." << std::endl;
            }
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "Warning: Failed to parse existing " << json_filename << ": " << e.what() << ". Overwriting." << std::endl;
            anomalies_vec.clear();
        } catch (const nlohmann::json::type_error& e) { 
            std::cerr << "Warning: Type error converting JSON item to HeartbeatAnomaly from " << json_filename << ": " << e.what() << ". Clearing previous data." << std::endl;
            anomalies_vec.clear();
        } catch (const nlohmann::json::out_of_range& e) {
            std::cerr << "Warning: Missing key in JSON item from " << json_filename << ": " << e.what() << ". Clearing previous data." << std::endl;
            anomalies_vec.clear();
        }
        infile.close();
    }

    anomalies_vec.push_back(new_anomaly_struct);

    std::sort(anomalies_vec.begin(), anomalies_vec.end(), [](const HeartbeatAnomaly& a, const HeartbeatAnomaly& b) {
        return std::abs(a.deviation) > std::abs(b.deviation);
    });

    nlohmann::json sorted_anomalies_json_array = anomalies_vec;

    std::ofstream outfile(json_filename, std::ios::trunc);
    if (!outfile) {
        std::cerr << "Error: Unable to open JSON file for writing: " << json_filename << std::endl;
        return;
    }
    outfile << sorted_anomalies_json_array.dump(4);
    if (!outfile) {
        std::cerr << "Error: Failed to write data to JSON file." << std::endl;
    }
}