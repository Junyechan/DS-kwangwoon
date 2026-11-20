#include <iostream>
#include <string>
#include "parking_list.h"
#include "search.h"

static int failed = 0;

#define CHECK(c) do { if (!(c)) { std::cout << "FAIL line " << __LINE__ << ": " #c "\n"; failed++; } } while (0)

static Parking make(const std::string& name, const std::string& district)
{
    Parking p;
    p.name = name; p.address = ""; p.district = district;
    p.capacity = 10; p.basicFee = 0; p.hourlyFee = 0;
    p.openTime = "0000"; p.closeTime = "2400";
    return p;
}

static void testDistrictSearch()
{
    ParkingList list, result;
    createParkingList(&list);
    createParkingList(&result);

    // 여러 자치구에 섞여 있는 다건 데이터 (원본 순서: A1 B1 A2 C1 A3)
    const char* names[] = {"A1", "B1", "A2", "C1", "A3"};
    const char* dists[] = {"노원구", "강남구", "노원구", "서초구", "노원구"};
    for (int i = 0; i < 5; i++)
    {
        Parking p = make(names[i], dists[i]);
        p.capacity = 100 + i;
        addParking(&list, p);
    }
    buildSearchIndex(&list);

    // 다건 결과: 모두 해당 자치구이고 원본 순서/필드 유지
    CHECK(searchByDistrict("노원구", &result));
    CHECK(result.size == 3);
    CHECK(result.head->data.name == "A1");
    CHECK(result.head->next->data.name == "A2");
    CHECK(result.tail->data.name == "A3" && result.tail->data.capacity == 104);
    for (ParkingNode* n = result.head; n; n = n->next)
        CHECK(n->data.district == "노원구");

    // 결과는 원본 노드와 별개의 복사본
    CHECK(result.head != list.head);

    // result에 남은 이전 값은 먼저 비워진다 (성공 / 실패 모두)
    CHECK(searchByDistrict("강남구", &result) && result.size == 1);
    CHECK(!searchByDistrict("제주도", &result));
    CHECK(result.size == 0 && result.head == nullptr && result.tail == nullptr);

    // 부분 일치 / 대소문자 / 공백은 일치하지 않는다 (정확 일치)
    CHECK(!searchByDistrict("노원", &result));
    CHECK(!searchByDistrict("노원구 ", &result));

    // result가 nullptr이어도 비정상 종료하지 않는다
    CHECK(!searchByDistrict("노원구", nullptr));

    // 인덱스 구축 전/해제 후에도 안전
    destroySearchIndex();
    CHECK(!searchByDistrict("노원구", &result));

    destroyParkingList(&list);
    destroyParkingList(&result);
}

static void testDistrictCollision()
{
    // 서로 다른 자치구 1000개 > 버킷 수 -> 충돌 발생이 보장된다.
    // 충돌한 key끼리 결과가 섞이지 않아야 한다.
    ParkingList list, result;
    createParkingList(&list);
    createParkingList(&result);

    for (int i = 0; i < 1000; i++)
        addParking(&list, make("N" + std::to_string(i), "D" + std::to_string(i)));
    buildSearchIndex(&list);

    bool ok = true;
    for (int i = 0; i < 1000; i++)
    {
        std::string d = "D" + std::to_string(i);
        if (!searchByDistrict(d, &result) || result.size != 1 ||
            result.head->data.name != "N" + std::to_string(i))
            ok = false;
    }
    CHECK(ok);

    // 없는 key: 서로 다른 key 1000개가 211개 버킷을 사실상 모두 채우므로
    // 없는 key도 사용 중인 버킷에 들어간다. 충돌한 key의 결과를 돌려주면 안 된다.
    bool absentOk = true;
    for (int i = 0; i < 1000; i++)
    {
        if (searchByDistrict("Z" + std::to_string(i), &result) || result.size != 0)
            absentOk = false;
    }
    CHECK(absentOk);

    destroySearchIndex();
    destroyParkingList(&list);
    destroyParkingList(&result);
}

static void testNameSearch()
{
    ParkingList list, result;
    createParkingList(&list);
    createParkingList(&result);

    // 동일 이름 2건(서로 다른 자치구)을 포함
    addParking(&list, make("서울역 주차장", "중구"));
    addParking(&list, make("노원역 주차장", "노원구"));
    addParking(&list, make("서울역 주차장", "용산구"));
    buildSearchIndex(&list);

    // 단건
    CHECK(searchByName("노원역 주차장", &result));
    CHECK(result.size == 1 && result.head->data.district == "노원구");
    CHECK(result.head != list.head->next);  // 원본 복사본

    // 동명 다건: 원본 순서 유지
    CHECK(searchByName("서울역 주차장", &result));
    CHECK(result.size == 2);
    CHECK(result.head->data.district == "중구");
    CHECK(result.tail->data.district == "용산구");

    // 없는 이름 / 부분 일치 / 빈 문자열: false + 빈 결과 (이전 결과도 비움)
    CHECK(!searchByName("없는 주차장", &result));
    CHECK(result.size == 0 && result.head == nullptr && result.tail == nullptr);
    searchByName("노원역 주차장", &result);
    CHECK(!searchByName("노원역", &result) && result.size == 0);
    CHECK(!searchByName("", &result));

    // result nullptr / 인덱스 해제 후
    CHECK(!searchByName("노원역 주차장", nullptr));
    destroySearchIndex();
    CHECK(!searchByName("노원역 주차장", &result));

    // 이름 검색 결과를 수정해도 인덱스/원본은 영향 없음
    buildSearchIndex(&list);
    searchByName("노원역 주차장", &result);
    result.head->data.name = "CHANGED";
    CHECK(list.head->next->data.name == "노원역 주차장");
    CHECK(searchByName("노원역 주차장", &result) && result.size == 1);

    destroySearchIndex();
    destroyParkingList(&list);
    destroyParkingList(&result);
}

static void testNameCollision()
{
    // 서로 다른 이름 1000개 > 버킷 수 -> 충돌 보장, 결과가 섞이면 안 됨
    ParkingList list, result;
    createParkingList(&list);
    createParkingList(&result);

    for (int i = 0; i < 1000; i++)
        addParking(&list, make("Name" + std::to_string(i), "D"));
    buildSearchIndex(&list);

    bool ok = true;
    for (int i = 0; i < 1000; i++)
    {
        if (!searchByName("Name" + std::to_string(i), &result) ||
            result.size != 1 ||
            result.head->data.name != "Name" + std::to_string(i))
            ok = false;
    }
    CHECK(ok);

    // 없는 key: 서로 다른 key 1000개가 211개 버킷을 사실상 모두 채우므로
    // 없는 key도 사용 중인 버킷에 들어간다. 충돌한 key의 결과를 돌려주면 안 된다.
    bool absentOk = true;
    for (int i = 0; i < 1000; i++)
    {
        if (searchByName("Z" + std::to_string(i), &result) || result.size != 0)
            absentOk = false;
    }
    CHECK(absentOk);

    destroySearchIndex();
    destroyParkingList(&list);
    destroyParkingList(&result);
}

static void testEmptyAndNull()
{
    destroySearchIndex();
    ParkingList list, result;
    createParkingList(&list);
    createParkingList(&result);

    buildSearchIndex(&list);  // 빈 리스트
    CHECK(!searchByDistrict("노원구", &result));
    CHECK(result.size == 0);
    buildSearchIndex(nullptr);
    CHECK(!searchByName("A", &result));

    destroySearchIndex();
    destroyParkingList(&list);
    destroyParkingList(&result);
}

static void testConcentratedKey()
{
    destroySearchIndex();
    ParkingList list, result;
    createParkingList(&list);
    createParkingList(&result);

    // 동일 자치구 집중 (같은 key 다수)
    for (int i = 0; i < 500; i++)
        addParking(&list, make("P" + std::to_string(i), "노원구"));
    addParking(&list, make("X", "강남구"));
    addParking(&list, make("", ""));  // 빈 key는 색인 제외

    buildSearchIndex(&list);
    CHECK(searchByDistrict("노원구", &result) && result.size == 500);
    CHECK(result.head->data.name == "P0");  // 원본 순서 유지
    CHECK(searchByDistrict("강남구", &result) && result.size == 1);
    CHECK(!searchByDistrict("없는구", &result) && result.size == 0);
    CHECK(!searchByDistrict("", &result));  // 빈 key는 검색 불가
    CHECK(!searchByName("", &result));
    CHECK(searchByName("P499", &result) && result.size == 1);
    CHECK(!searchByName("없음", &result));

    destroySearchIndex();
    destroyParkingList(&list);
    destroyParkingList(&result);
}

static void testResultIndependence()
{
    destroySearchIndex();
    ParkingList list, result;
    createParkingList(&list);
    createParkingList(&result);

    addParking(&list, make("X", "강남구"));
    buildSearchIndex(&list);

    // 결과를 변경해도 원본/색인에 영향 없음
    searchByDistrict("강남구", &result);
    result.head->data.name = "CHANGED";
    CHECK(list.head->data.name == "X");
    CHECK(searchByName("X", &result) && result.size == 1);

    destroySearchIndex();
    destroyParkingList(&list);
    destroyParkingList(&result);
}

static void testRebuildAndDoubleDestroy()
{
    destroySearchIndex();
    ParkingList list, result;
    createParkingList(&list);
    createParkingList(&result);

    addParking(&list, make("A", "노원구"));
    buildSearchIndex(&list);
    buildSearchIndex(&list);  // 재구축해도 중복 색인되지 않는다
    CHECK(searchByDistrict("노원구", &result) && result.size == 1);

    destroySearchIndex();
    destroySearchIndex();  // 중복 해제 안전
    CHECK(!searchByDistrict("노원구", &result));

    destroyParkingList(&list);
    destroyParkingList(&result);
}

int main()
{
    testEmptyAndNull();
    testConcentratedKey();
    testResultIndependence();
    testRebuildAndDoubleDestroy();
    testDistrictSearch();
    testDistrictCollision();
    testNameSearch();
    testNameCollision();

    std::cout << (failed ? "FAILED\n" : "ALL PASSED\n");
    return failed;
}
