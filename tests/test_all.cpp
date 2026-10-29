// 간단한 단위 테스트 (외부 라이브러리 없이 실행)
// 빌드: g++ -std=c++11 -Isrc tests/test_all.cpp src/parking_list.cpp src/data_loader.cpp src/search.cpp src/sort.cpp src/recommendation.cpp -o run_tests
#include <iostream>
#include <string>
#include <cmath>

#include "parking_list.h"
#include "data_loader.h"
#include "search.h"
#include "sort.h"
#include "recommendation.h"

static int failures = 0;

#define CHECK(cond)                                              \
    do                                                           \
    {                                                            \
        if (!(cond))                                             \
        {                                                        \
            std::cout << "  FAIL " << __LINE__ << ": " #cond "\n"; \
            failures++;                                          \
        }                                                        \
    } while (0)

static Parking makeParking(const std::string& name, int fee, int capacity)
{
    Parking p;
    p.name = name;
    p.district = "테스트구";
    p.capacity = capacity;
    p.basicFee = fee;
    return p;
}

static void testSortEmptyAndSingle()
{
    std::cout << "[sort] 빈 리스트 / 노드 1개\n";

    ParkingList list;
    createParkingList(&list);

    sortByFee(&list);
    CHECK(list.head == nullptr && list.tail == nullptr);

    addParking(&list, makeParking("A", 1000, 10));
    sortByCapacity(&list);
    CHECK(list.head == list.tail && list.size == 1);

    destroyParkingList(&list);
}

static void testSortByFee()
{
    std::cout << "[sort] 요금 오름차순 + 동일 값 순서 유지 + tail 갱신\n";

    ParkingList list;
    createParkingList(&list);

    addParking(&list, makeParking("A", 3000, 10));
    addParking(&list, makeParking("B", 1000, 20));
    addParking(&list, makeParking("C", 2000, 30));
    addParking(&list, makeParking("D", 1000, 40));
    addParking(&list, makeParking("E", 500, 50));

    sortByFee(&list);

    const char* expected[] = { "E", "B", "D", "C", "A" };
    ParkingNode* cur = list.head;

    for (int i = 0; i < 5; i++, cur = cur->next)
        CHECK(cur != nullptr && cur->data.name == expected[i]);

    CHECK(cur == nullptr);
    CHECK(list.tail->data.name == "A" && list.tail->next == nullptr);
    CHECK(list.size == 5);

    destroyParkingList(&list);
}

static void testSortByCapacity()
{
    std::cout << "[sort] 주차면수 내림차순\n";

    ParkingList list;
    createParkingList(&list);

    addParking(&list, makeParking("A", 0, 50));
    addParking(&list, makeParking("B", 0, 300));
    addParking(&list, makeParking("C", 0, 100));
    addParking(&list, makeParking("D", 0, 200));

    sortByCapacity(&list);

    int expected[] = { 300, 200, 100, 50 };
    ParkingNode* cur = list.head;

    for (int i = 0; i < 4; i++, cur = cur->next)
        CHECK(cur->data.capacity == expected[i]);

    CHECK(list.tail->data.capacity == 50);

    destroyParkingList(&list);
}

static void testSortFeeUnknownLast()
{
    std::cout << "[sort] 요금 정보 없음(FEE_UNKNOWN)은 맨 뒤, 그 안에서도 순서 유지\n";

    ParkingList list;
    createParkingList(&list);

    addParking(&list, makeParking("U1", FEE_UNKNOWN, 10));
    addParking(&list, makeParking("A", 2000, 10));
    addParking(&list, makeParking("U2", FEE_UNKNOWN, 10));
    addParking(&list, makeParking("B", 0, 10));

    sortByFee(&list);

    const char* expected[] = { "B", "A", "U1", "U2" };
    ParkingNode* cur = list.head;

    for (int i = 0; i < 4; i++, cur = cur->next)
        CHECK(cur != nullptr && cur->data.name == expected[i]);

    destroyParkingList(&list);
}

static void testSortCapacityStable()
{
    std::cout << "[sort] 주차면수 같은 값은 입력 순서 유지\n";

    ParkingList list;
    createParkingList(&list);

    addParking(&list, makeParking("A", 0, 100));
    addParking(&list, makeParking("B", 0, 300));
    addParking(&list, makeParking("C", 0, 100));
    addParking(&list, makeParking("D", 0, 300));

    sortByCapacity(&list);

    const char* expected[] = { "B", "D", "A", "C" };
    ParkingNode* cur = list.head;

    for (int i = 0; i < 4; i++, cur = cur->next)
        CHECK(cur != nullptr && cur->data.name == expected[i]);

    destroyParkingList(&list);
}

// result가 sortByFee(list)의 앞 result.size개와 이름까지 같은지
static bool matchesSortedPrefix(const ParkingList* list, const ParkingList* result)
{
    ParkingList copy;
    createParkingList(&copy);

    for (ParkingNode* n = list->head; n != nullptr; n = n->next)
        addParking(&copy, n->data);

    sortByFee(&copy);

    bool same = true;
    ParkingNode* a = copy.head;
    ParkingNode* b = result->head;

    for (; b != nullptr; a = a->next, b = b->next)
    {
        if (a == nullptr || a->data.name != b->data.name)
        {
            same = false;
            break;
        }
    }

    destroyParkingList(&copy);

    return same;
}

static void testRecommendTiesAndUnknown()
{
    std::cout << "[recommend] 동점은 입력 순서 유지(정렬 결과와 동일), 요금 정보 없음은 제외\n";

    ParkingList list;
    createParkingList(&list);

    // 1000원이 여러 개: 입력 순서 B, D, F, G로 나와야 한다.
    addParking(&list, makeParking("A", 3000, 10));
    addParking(&list, makeParking("B", 1000, 10));
    addParking(&list, makeParking("C", FEE_UNKNOWN, 10));
    addParking(&list, makeParking("D", 1000, 10));
    addParking(&list, makeParking("E", 500, 10));
    addParking(&list, makeParking("F", 1000, 10));
    addParking(&list, makeParking("G", 1000, 10));

    ParkingList result;
    createParkingList(&result);

    CHECK(recommendTopNByFee(&list, &result, 4));

    const char* expected[] = { "E", "B", "D", "F" };
    ParkingNode* cur = result.head;

    for (int i = 0; i < 4; i++, cur = cur->next)
        CHECK(cur != nullptr && cur->data.name == expected[i]);

    for (int k = 1; k <= 7; k++)
    {
        clearParkingList(&result);
        recommendTopNByFee(&list, &result, k);
        CHECK(matchesSortedPrefix(&list, &result));
    }

    // 전부 달라고 해도 요금 정보 없는 C는 빠져 6개
    clearParkingList(&result);
    CHECK(recommendTopNByFee(&list, &result, 100));
    CHECK(result.size == 6);

    destroyParkingList(&result);
    destroyParkingList(&list);
}

static void testLoaderAndSearch()
{
    std::cout << "[loader/search] 샘플 CSV 로딩 + 검색\n";

    ParkingList all;
    createParkingList(&all);

    CHECK(!loadParkingData("data/없는파일.csv", &all));
    CHECK(loadParkingData("tests/sample_parking.csv", &all));
    CHECK(all.size == 26);
    CHECK(all.head->data.district == "광진구");
    CHECK(all.head->data.openTime == "09:00");

    buildSearchIndex(&all);

    ParkingList result;
    createParkingList(&result);

    CHECK(searchByDistrict("광진구", &result));
    CHECK(result.size == 5);
    clearParkingList(&result);

    CHECK(searchByName("송파 \"잠실\" 공영주차장", &result));
    CHECK(result.size == 1);
    clearParkingList(&result);

    CHECK(!searchByDistrict("없는구", &result));
    CHECK(result.size == 0);

    destroySearchIndex();
    destroyParkingList(&result);
    destroyParkingList(&all);
}

static void testRealData()
{
    std::cout << "[loader] 실제 서울시 데이터 (data/parking.csv)\n";

    ParkingList all;
    createParkingList(&all);

    CHECK(loadParkingData("data/parking.csv", &all));

    // 2181행 → 주차장코드 기준 842개 (영문 컬럼명 행 제외, 중복 코드 병합)
    CHECK(all.size == 842);

    buildSearchIndex(&all);

    ParkingList result;
    createParkingList(&result);

    // 노상 주차장: 주차면 1개당 1행 → 84행이 1개로 합쳐져야 함
    CHECK(searchByName("여의서로 공영주차장(시)", &result));
    CHECK(result.size == 1);
    CHECK(result.head->data.capacity == 84);
    CHECK(result.head->data.district == "영등포구");
    CHECK(result.head->data.basicFee == 3360);  // 5분당 280원
    clearParkingList(&result);

    // 기본 시간 0분 + 5분당 100원 → 1시간 1200원 (예전에는 0원으로 무료처럼 보였음)
    CHECK(searchByName("쌍문2동 5공영주차장(구)", &result));
    CHECK(result.size == 1 && result.head->data.basicFee == 1200);
    clearParkingList(&result);

    CHECK(!searchByName("prknglot_nm", &result));

    // 유료인데 1시간 요금이 0원으로 계산되는 주차장 8곳 → 요금 정보 없음
    int unknown = 0;

    for (ParkingNode* n = all.head; n != nullptr; n = n->next)
        if (n->data.basicFee == FEE_UNKNOWN)
            unknown++;

    CHECK(unknown == 8);

    // 전체 842개 Top-N == 요금순 정렬 앞 N개
    int ks[] = { 1, 5, 10, 50, 842 };

    for (int i = 0; i < 5; i++)
    {
        recommendTopNByFee(&all, &result, ks[i]);
        CHECK(matchesSortedPrefix(&all, &result));
        clearParkingList(&result);
    }

    // 정렬 결과 검증 + 비교 횟수가 n log2 n 이하인지
    sortByCapacity(&all);

    bool ordered = true;

    for (ParkingNode* n = all.head; n->next != nullptr; n = n->next)
        if (n->data.capacity < n->next->data.capacity)
            ordered = false;

    CHECK(ordered);
    CHECK(getLastSortComparisons() <= (long long)(842 * std::log2(842.0)));

    destroySearchIndex();
    destroyParkingList(&result);
    destroyParkingList(&all);
}

static void testRecommend()
{
    std::cout << "[recommend] Top-N\n";

    ParkingList list;
    createParkingList(&list);

    int fees[] = { 3000, 500, 2000, 1000, 1500, 800 };

    for (int i = 0; i < 6; i++)
        addParking(&list, makeParking(std::string(1, 'A' + i), fees[i], 10));

    ParkingList result;
    createParkingList(&result);

    CHECK(recommendTopNByFee(&list, &result, 3));
    CHECK(result.size == 3);
    CHECK(result.head->data.basicFee == 500);
    CHECK(result.head->next->data.basicFee == 800);
    CHECK(result.tail->data.basicFee == 1000);
    clearParkingList(&result);

    CHECK(recommendTopNByFee(&list, &result, 100));
    CHECK(result.size == 6);
    clearParkingList(&result);

    CHECK(!recommendTopNByFee(&list, &result, 0));

    ParkingList empty;
    createParkingList(&empty);
    CHECK(!recommendTopNByFee(&empty, &result, 3));

    destroyParkingList(&result);
    destroyParkingList(&list);
}

int main()
{
    testSortEmptyAndSingle();
    testSortByFee();
    testSortByCapacity();
    testLoaderAndSearch();
    testSortFeeUnknownLast();
    testSortCapacityStable();
    testRecommend();
    testRecommendTiesAndUnknown();
    testRealData();

    if (failures == 0)
        std::cout << "\n모든 테스트 통과\n";
    else
        std::cout << "\n실패: " << failures << "개\n";

    return failures == 0 ? 0 : 1;
}
