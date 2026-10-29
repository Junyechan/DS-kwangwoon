#ifndef SEARCH_H
#define SEARCH_H

#include <string>
#include "parking_list.h"

// 원본 ParkingList를 한 번 순회하며
// 자치구 / 주차장명 Hash Table을 구축한다.
void buildSearchIndex(
    const ParkingList* list
);

// 자치구가 일치하는 주차장을 result에 복사한다.
// 결과가 하나 이상이면 true
bool searchByDistrict(
    const std::string& district,
    ParkingList* result
);

// 주차장명이 정확히 일치하는 주차장을 result에 복사한다.
bool searchByName(
    const std::string& name,
    ParkingList* result
);

// Hash Table 메모리 해제
void destroySearchIndex();

#endif
