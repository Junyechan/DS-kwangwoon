#ifndef PARKING_H
#define PARKING_H

#include <string>

struct Parking
{
    std::string name;       // 주차장명
    std::string address;    // 주소
    std::string district;   // 자치구

    int capacity;           // 주차면수
    int basicFee;           // 기본 요금

    std::string openTime;   // 운영 시작 시간
    std::string closeTime;  // 운영 종료 시간
};

#endif
