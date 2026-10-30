// 정렬 / Top-N 추천의 비교 횟수와 실행 시간을 n을 키워 가며 측정한다.
// 빌드: g++ -std=c++11 -O2 -Isrc tests/benchmark.cpp src/parking_list.cpp src/data_loader.cpp src/sort.cpp src/recommendation.cpp -o benchmark
#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <cstdlib>

#include "parking_list.h"
#include "data_loader.h"
#include "sort.h"
#include "recommendation.h"

typedef std::chrono::steady_clock Clock;

static double elapsedMs(Clock::time_point start)
{
    return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}

// 요금 0~5000원(100원 단위), 주차면수 1~500면을 무작위로 채운 리스트
static void fillRandom(ParkingList* list, int n)
{
    for (int i = 0; i < n; i++)
    {
        Parking p;
        p.name = "P";
        p.capacity = 1 + std::rand() % 500;
        p.hourlyFee = (std::rand() % 51) * 100;
        addParking(list, p);
    }
}

// 요금이 점점 싸지는 리스트: 새 후보가 매번 Heap 루트를 밀어내는 최악의 경우
static void fillDescending(ParkingList* list, int n)
{
    for (int i = 0; i < n; i++)
    {
        Parking p;
        p.name = "P";
        p.capacity = 1;
        p.hourlyFee = n - i;
        addParking(list, p);
    }
}

static void copyList(const ParkingList* from, ParkingList* to)
{
    for (ParkingNode* n = from->head; n != nullptr; n = n->next)
        addParking(to, n->data);
}

static void measure(const char* label, const ParkingList* source, int topN)
{
    int n = source->size;

    ParkingList work;
    createParkingList(&work);
    copyList(source, &work);

    Clock::time_point start = Clock::now();
    sortByFee(&work);
    double sortMs = elapsedMs(start);
    long long sortCmp = getLastSortComparisons();

    ParkingList top;
    createParkingList(&top);

    start = Clock::now();
    recommendTopNByFee(source, &top, topN);
    double heapMs = elapsedMs(start);
    long long heapCmp = getLastRecommendComparisons();

    std::cout
        << std::left << std::setw(10) << label
        << std::right << std::setw(9) << n
        << std::setw(14) << sortCmp
        << std::setw(14) << (long long)(n * std::log2((double)n))
        << std::setw(8) << std::fixed << std::setprecision(3)
        << (double)sortCmp / (n * std::log2((double)n))
        << std::setw(10) << std::setprecision(2) << sortMs
        << std::setw(12) << heapCmp
        << std::setw(10) << heapMs
        << "\n";

    destroyParkingList(&top);
    destroyParkingList(&work);
}

int main()
{
    const int topN = 10;

    std::srand(20261103);

    std::cout
        << "Top-N은 N = " << topN << "\n"
        << std::left << std::setw(10) << "데이터"
        << std::right << std::setw(9) << "n"
        << std::setw(14) << "정렬 비교"
        << std::setw(14) << "n log2 n"
        << std::setw(8) << "비율"
        << std::setw(10) << "정렬 ms"
        << std::setw(12) << "Heap 비교"
        << std::setw(10) << "Heap ms"
        << "\n";

    ParkingList real;
    createParkingList(&real);

    if (loadParkingData("data/parking.csv", &real))
        measure("실제", &real, topN);

    destroyParkingList(&real);

    for (int n = 1000; n <= 256000; n *= 2)
    {
        ParkingList list;
        createParkingList(&list);
        fillRandom(&list, n);
        measure("무작위", &list, topN);
        destroyParkingList(&list);
    }

    for (int n = 1000; n <= 256000; n *= 4)
    {
        ParkingList list;
        createParkingList(&list);
        fillDescending(&list, n);
        measure("역순", &list, topN);
        destroyParkingList(&list);
    }

    return 0;
}
