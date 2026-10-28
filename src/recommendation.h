#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include "parking_list.h"

bool recommendTopNByFee(
    const ParkingList* candidates,
    ParkingList* result,
    int topN
);

#endif
