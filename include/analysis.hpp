#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP

#include "HeartBeatData.h" // Menggunakan struktur data baru

// Deklarasi fungsi isAnomaly
bool isAnomaly(const HeartbeatRecord& data, HeartbeatAnomaly& outRecord);

#endif // ANALYSIS_HPP