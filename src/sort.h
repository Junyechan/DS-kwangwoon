#ifndef SORT_H
#define SORT_H

#include "parking_list.h"

void sortByFee(
    ParkingList* list
);

void sortByCapacity(
    ParkingList* list
);

// 직전 sortByFee / sortByCapacity 호출에서 일어난 원소 비교 횟수
long long getLastSortComparisons();

#endif
