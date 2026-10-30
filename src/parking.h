#ifndef PARKING_H
#define PARKING_H

#include <string>

// hourlyFee 값으로 쓰는 "요금 정보 없음" 표시 (유료인데 원본 요금이 비어 있는 주차장)
const int FEE_UNKNOWN = -1;

struct Parking
{
    std::string name;
    std::string address;
    std::string district;

    int capacity;
    int basicFee;   // 기본 주차 요금(원), 원본 CSV 값 그대로
    int hourlyFee;  // 1시간 주차 시 실제 요금(원), 모르면 FEE_UNKNOWN

    std::string openTime;
    std::string closeTime;
};

#endif
