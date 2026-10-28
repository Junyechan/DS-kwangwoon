#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <string>
#include <vector>

#include "parking.h"

std::vector<Parking> loadParkingData(
    const std::string& filename
);

#endif
