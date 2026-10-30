#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include "parking_list.h"

// result를 비운 뒤, 1시간 요금(hourlyFee)이 낮은 순으로 최대 topN개를 담는다.
// 요금이 같으면 candidates에서 먼저 나온 순서를 유지하고, 요금 정보 없는 주차장은 제외한다.
bool recommendTopNByFee(
    const ParkingList* candidates,
    ParkingList* result,
    int topN
);

// 직전 recommendTopNByFee 호출에서 일어난 원소 비교 횟수
long long getLastRecommendComparisons();

#endif
