#include <iostream>

#include "parking.h"
#include "parking_list.h"
#include "data_loader.h"
#include "search.h"
#include "sort.h"
#include "recommendation.h"

int main()
{
    ParkingList parkingLots;

    createParkingList(&parkingLots);

    if (!loadParkingData(
            "data/parking.csv",
            &parkingLots))
    {
        std::cout
            << "데이터 로딩 실패\n";

        return 1;
    }

    // 메뉴
    // 검색
    // 정렬
    // 추천
    // 출력

    destroyParkingList(&parkingLots);

    return 0;
}
