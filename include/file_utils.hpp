#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include "HeartBeatData.h"


void saveToBinary(const HeartbeatRecord& data);
void saveToJSON(const HeartbeatAnomaly& anomaly);

#endif