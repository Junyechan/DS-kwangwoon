#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include <vector>

#include "parking.h"

std::vector<Parking> recommendTopNByFee(
    const std::vector<Parking>& parkingLots,
    int topN
);

#endif
