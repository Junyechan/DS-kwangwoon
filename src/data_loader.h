#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <string>
#include <vector>

#include "parking.h"

bool loadParkingData(
    const std::string& filename,
    ParkingList* list
);

#endif
