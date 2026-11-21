// 검색(Hash Table) 측정: 버킷 체인 분포와 searchByName 조회 시간을 순차 탐색과 비교한다.
// 빌드: g++ -std=c++20 -O2 -Isrc tests/benchmark_search.cpp src/parking_list.cpp src/data_loader.cpp src/search.cpp -o benchmark_search
// 실행: ./benchmark_search [csv 경로]   (기본값 data/parking.csv)
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>

#include "parking_list.h"
#include "data_loader.h"
#include "search.h"

typedef std::chrono::steady_clock Clock;

static const int TABLE_SIZE = 211;   // search.cpp와 같은 값
static const int REPEAT = 20000;

// search.cpp의 djb2 해시와 동일 (체인 분포 계산용)
static unsigned int hashString(const std::string& key)
{
    unsigned int hash = 5381;
    for (size_t i = 0; i < key.size(); i++)
        hash = hash * 33 + (unsigned char)key[i];
    return hash % TABLE_SIZE;
}

// 구현과 같은 기준: Parking 1건당 노드 1개. 체인 길이 = 버킷에 든 노드 수
static void chainStats(const char* label, const ParkingList* list, bool byName)
{
    int nodes[TABLE_SIZE] = {};
    int total = 0;
    for (ParkingNode* p = list->head; p != nullptr; p = p->next)
    {
        const std::string& key = byName ? p->data.name : p->data.district;
        if (key.empty()) continue;
        nodes[hashString(key)]++;
        total++;
    }

    int used = 0, longest = 0;
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (nodes[i] > 0) used++;
        if (nodes[i] > longest) longest = nodes[i];
    }

    std::cout << std::left << std::setw(8) << label
              << " 노드 수 " << total
              << " | 사용 버킷 " << used << "/" << TABLE_SIZE
              << " | 평균 체인 길이 " << std::fixed << std::setprecision(2)
              << (double)total / TABLE_SIZE
              << " | 가장 긴 체인 " << longest << "\n";
}

int main(int argc, char* argv[])
{
    std::string path = (argc > 1) ? argv[1] : "data/parking.csv";

    ParkingList list;
    createParkingList(&list);
    if (!loadParkingData(path, &list))
    {
        std::cerr << "데이터를 읽지 못했습니다: " << path << "\n";
        return 1;
    }
    std::cout << "n = " << list.size << "\n";

    chainStats("자치구", &list, false);
    chainStats("주차장명", &list, true);

    // 리스트 맨 끝 이름 (순차 탐색에 가장 불리한 경우)
    ParkingNode* last = list.head;
    while (last->next != nullptr) last = last->next;
    const std::string target = last->data.name;

    buildSearchIndex(&list);

    ParkingList result;
    createParkingList(&result);

    long long sink = 0;   // 최적화로 반복문이 사라지지 않게 결과를 누적

    Clock::time_point t0 = Clock::now();
    for (int i = 0; i < REPEAT; i++)
    {
        searchByName(target, &result);
        sink += result.size;
    }
    double hashUs = std::chrono::duration<double, std::micro>(Clock::now() - t0).count() / REPEAT;

    t0 = Clock::now();
    for (int i = 0; i < REPEAT; i++)
    {
        clearParkingList(&result);
        for (ParkingNode* p = list.head; p != nullptr; p = p->next)
            if (p->data.name == target)
                addParking(&result, p->data);
        sink += result.size;
    }
    double seqUs = std::chrono::duration<double, std::micro>(Clock::now() - t0).count() / REPEAT;

    std::cout << std::fixed << std::setprecision(2)
              << "검색 대상: " << target << " (반복 " << REPEAT << "회)\n"
              << "Hash Table searchByName : " << hashUs << " us\n"
              << "순차 탐색(복사 포함)    : " << seqUs << " us\n"
              << "배율                    : " << seqUs / hashUs << "x\n"
              << "(checksum " << sink << ")\n";

    destroySearchIndex();
    destroyParkingList(&result);
    destroyParkingList(&list);
    return 0;
}
