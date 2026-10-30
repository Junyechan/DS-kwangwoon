#include <iostream>
#include <string>
#include <cstdlib>

#include "parking.h"
#include "parking_list.h"
#include "data_loader.h"
#include "search.h"
#include "sort.h"
#include "recommendation.h"

static std::string readLine(const std::string& prompt)
{
    std::string line;

    std::cout << prompt;

    if (!std::getline(std::cin, line))
        return "0";

    return line;
}

// 숫자가 아닌 입력(빈 줄, "abc", "1a")은 -1을 반환한다.
// atoi는 이런 입력을 0으로 바꿔서 "0. 종료"로 처리되어 버린다.
static int readInt(const std::string& prompt)
{
    std::string line = readLine(prompt);

    size_t start = line.find_first_not_of(" \t\r");
    size_t end = line.find_last_not_of(" \t\r");

    if (start == std::string::npos)
        return -1;

    std::string digits = line.substr(start, end - start + 1);

    if (digits.size() > 6)
        return -1;

    for (size_t i = 0; i < digits.size(); i++)
    {
        if (digits[i] < '0' || digits[i] > '9')
            return -1;
    }

    return std::atoi(digits.c_str());
}

static void printParking(int index, const Parking& p)
{
    std::cout
        << index << ". " << p.name << "\n"
        << "   주소: " << p.address
        << " (" << p.district << ")\n"
        << "   주차면수: " << p.capacity << "면"
        << " | 1시간 요금: ";

    if (p.hourlyFee == FEE_UNKNOWN)
        std::cout << "정보 없음";
    else
        std::cout << p.hourlyFee << "원";

    std::cout
        << " | 운영시간: " << p.openTime << " ~ " << p.closeTime
        << "\n";
}

static void printParkingList(const ParkingList* list)
{
    if (isParkingListEmpty(list))
    {
        std::cout << "결과가 없습니다.\n";
        return;
    }

    int index = 1;
    ParkingNode* current = list->head;

    while (current != nullptr)
    {
        printParking(index++, current->data);
        current = current->next;
    }

    std::cout << "총 " << list->size << "개\n";
}

// 검색 결과에 대해 정렬 / 추천을 반복해서 적용하는 메뉴
static void resultMenu(ParkingList* result)
{
    while (true)
    {
        std::cout
            << "\n[검색 결과 메뉴]\n"
            << "1. 1시간 요금 낮은 순 정렬\n"
            << "2. 주차면수 많은 순 정렬\n"
            << "3. 요금 기준 Top-N 추천\n"
            << "0. 메인 메뉴로\n";

        int choice = readInt("선택: ");

        if (choice == 1)
        {
            sortByFee(result);
            printParkingList(result);
            std::cout << "(Merge Sort 비교 " << getLastSortComparisons() << "회)\n";
        }
        else if (choice == 2)
        {
            sortByCapacity(result);
            printParkingList(result);
            std::cout << "(Merge Sort 비교 " << getLastSortComparisons() << "회)\n";
        }
        else if (choice == 3)
        {
            int topN = readInt("추천 개수(N): ");

            ParkingList recommendation;

            createParkingList(&recommendation);

            if (recommendTopNByFee(result, &recommendation, topN))
            {
                printParkingList(&recommendation);
                std::cout << "(Heap 비교 " << getLastRecommendComparisons() << "회)\n";
            }
            else
                std::cout << "추천 개수는 1 이상이어야 합니다.\n";

            destroyParkingList(&recommendation);
        }
        else if (choice == 0)
        {
            return;
        }
        else
        {
            std::cout << "잘못된 입력입니다.\n";
        }
    }
}

int main(int argc, char* argv[])
{
    std::string filename = "data/parking.csv";

    if (argc > 1)
        filename = argv[1];

    ParkingList parkingLots;

    createParkingList(&parkingLots);

    if (!loadParkingData(
            filename,
            &parkingLots))
    {
        std::cout
            << "데이터 로딩 실패\n";

        return 1;
    }

    buildSearchIndex(&parkingLots);

    std::cout
        << "주차하고 싶어요! - 서울시 공영주차장 "
        << parkingLots.size << "개 로딩 완료\n";

    while (true)
    {
        std::cout
            << "\n[메인 메뉴]\n"
            << "1. 전체 주차장 목록\n"
            << "2. 자치구로 검색\n"
            << "3. 주차장명으로 검색\n"
            << "0. 종료\n";

        int choice = readInt("선택: ");

        if (choice == 0)
            break;

        if (choice == 1)
        {
            printParkingList(&parkingLots);
            continue;
        }

        if (choice != 2 && choice != 3)
        {
            std::cout << "잘못된 입력입니다.\n";
            continue;
        }

        ParkingList result;

        createParkingList(&result);

        bool found;

        if (choice == 2)
            found = searchByDistrict(
                readLine("자치구 (예: 광진구): "), &result);
        else
            found = searchByName(
                readLine("주차장명: "), &result);

        if (found)
        {
            printParkingList(&result);
            resultMenu(&result);
        }
        else
        {
            std::cout << "검색 결과가 없습니다.\n";
        }

        destroyParkingList(&result);
    }

    destroySearchIndex();
    destroyParkingList(&parkingLots);

    return 0;
}
