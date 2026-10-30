# DS-kwangwoon 구현 가이드라인

본 문서는 서울시 공영주차장 검색 및 안내 프로그램의 구현 방향을 통일하고, 팀원별 모듈 간 충돌을 최소화하기 위한 개발 가이드입니다.

각 담당자는 자신의 기능을 독립적으로 구현하되, 공통 데이터 구조와 함수 인터페이스를 임의로 변경하지 않습니다.

---

# 1. 전체 프로그램 구조

프로그램은 다음 흐름을 기준으로 구현합니다.

```text
서울시 공영주차장 데이터
        ↓
데이터 로딩
        ↓
Parking 객체 생성
        ↓
Linked List에 전체 데이터 저장
        ↓
Hash Table 검색 인덱스 구축
        ↓
사용자 검색 조건 입력
        ↓
자치구 / 주차장명 검색
        ↓
검색 결과 생성
        ↓
Merge Sort / Heap
        ↓
콘솔 결과 출력
```

전체 공영주차장 데이터는 `ParkingList`라는 Linked List에 저장합니다.

프로그램의 메뉴 선택과 최종 결과 출력은 `main.cpp`에서 처리합니다.

---

# 2. 담당 역할

| 역할 | 주요 담당 |
|---|---|
| 조장 / 통합 | 공통 `Parking` 및 Linked List 구조 설계, 인터페이스 정의, `main.cpp`, 전체 모듈 통합, GitHub 관리 |
| 데이터 | 서울 데이터 허브 데이터 분석, 데이터 로딩, `ParkingList` 삽입 |
| 검색 | Hash Table 구축, 자치구 / 주차장명 검색 |
| 정렬 / 추천 | Linked List 정렬, Heap 기반 Top-N 추천 |
| 발표 / 테스트 | 기능 테스트, 예외 상황 검증, README 정리, 발표자료 작성 및 최종 발표 |

---

# 3. 공통 Parking 데이터 구조

모든 기능은 동일한 `Parking` 구조체를 사용합니다.

`src/parking.h`

```cpp
#ifndef PARKING_H
#define PARKING_H

#include <string>

struct Parking
{
    std::string name;       // 주차장명
    std::string address;    // 주소
    std::string district;   // 자치구

    int capacity;           // 주차면수
    int basicFee;           // 기본 주차 요금 (원본 CSV 값 그대로)
    int hourlyFee;          // 1시간 주차 시 실제 요금 (기본 + 추가 단위 요금), 모르면 FEE_UNKNOWN(-1)

    std::string openTime;   // 운영 시작 시간
    std::string closeTime;  // 운영 종료 시간
};

#endif
```

※ 위 필드는 서울 데이터 허브의 실제 데이터 컬럼 분석 후 최종 확정합니다.

## 공통 규칙

`Parking` 구조체는 모든 모듈에서 공통으로 사용합니다.

다른 팀원이 필요한 필드를 임의로 추가하거나 삭제하지 않습니다.

변경이 필요한 경우:

```text
Parking 구조 변경 필요
        ↓
Issue 또는 팀 채팅 공유
        ↓
조장 + 데이터 담당 확인
        ↓
parking.h 수정
        ↓
관련 팀원에게 변경사항 공유
```

---

# 4. 공통 Linked List 구조

기존 `std::vector<Parking>` 대신 직접 구현한 Linked List를 사용합니다.

`src/parking_list.h`

```cpp
#ifndef PARKING_LIST_H
#define PARKING_LIST_H

#include "parking.h"

struct ParkingNode
{
    Parking data;
    ParkingNode* next;
};

struct ParkingList
{
    ParkingNode* head;
    ParkingNode* tail;
    int size;
};

void createParkingList(ParkingList* list);

bool addParking(
    ParkingList* list,
    const Parking& parking
);

bool isParkingListEmpty(
    const ParkingList* list
);

void clearParkingList(
    ParkingList* list
);

void destroyParkingList(
    ParkingList* list
);

#endif
```

## Linked List 역할

```text
ParkingList

head
 ↓
[Parking] → [Parking] → [Parking] → ... → nullptr
                                      ↑
                                     tail
```

- `head`: 첫 번째 주차장 노드
- `tail`: 마지막 주차장 노드
- `size`: 저장된 주차장 개수

`tail`을 유지하여 데이터 로딩 시 새로운 주차장을 리스트 마지막에 빠르게 삽입할 수 있도록 합니다.

---

# 5. 프로젝트 파일 구조

```text
DS-kwangwoon/
│
├── README.md
├── .gitignore
│
├── .github/
│   └── pull_request_template.md
│
├── src/
│   ├── main.cpp
│   │
│   ├── parking.h
│   │
│   ├── parking_list.h
│   ├── parking_list.cpp
│   │
│   ├── data_loader.h
│   ├── data_loader.cpp
│   │
│   ├── search.h
│   ├── search.cpp
│   │
│   ├── sort.h
│   ├── sort.cpp
│   │
│   ├── recommendation.h
│   └── recommendation.cpp
│
├── data/
│
├── tests/
│
└── docs/
    └── IMPLEMENTATION_GUIDE.md
```

각 모듈의 역할은 다음과 같습니다.

```text
parking
→ 하나의 공영주차장 데이터 정의

parking_list
→ 전체 Parking 데이터 Linked List 관리

data_loader
→ 파일을 읽고 ParkingList에 데이터 삽입

search
→ Hash Table 구축 및 검색

sort
→ Linked List 검색 결과 정렬

recommendation
→ Heap 기반 Top-N 추천

main
→ 사용자 입력
→ 각 모듈 호출
→ 결과 출력

tests
→ 기능 검증
```

---

# 6. 조장 / 공통 Linked List 구현

## 담당 파일

```text
src/parking.h
src/parking_list.h
src/parking_list.cpp
src/main.cpp
```

조장은 팀원들이 공통으로 사용할 Linked List 구조를 먼저 구현합니다.

## 기본 구현 예시

`parking_list.cpp`

```cpp
#include "parking_list.h"

void createParkingList(ParkingList* list)
{
    if (list == nullptr)
        return;

    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
}

bool addParking(
    ParkingList* list,
    const Parking& parking
)
{
    if (list == nullptr)
        return false;

    ParkingNode* newNode = new ParkingNode;

    newNode->data = parking;
    newNode->next = nullptr;

    if (list->head == nullptr)
    {
        list->head = newNode;
        list->tail = newNode;
    }
    else
    {
        list->tail->next = newNode;
        list->tail = newNode;
    }

    list->size++;

    return true;
}

bool isParkingListEmpty(
    const ParkingList* list
)
{
    if (list == nullptr)
        return true;

    return list->head == nullptr;
}

void clearParkingList(ParkingList* list)
{
    if (list == nullptr)
        return;

    ParkingNode* current = list->head;

    while (current != nullptr)
    {
        ParkingNode* next = current->next;

        delete current;

        current = next;
    }

    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
}

void destroyParkingList(ParkingList* list)
{
    clearParkingList(list);
}
```

## 중요

`ParkingList`에 데이터를 추가할 때 각 담당자가 직접 노드를 생성하기보다는 가능한 한 공통 함수인

```cpp
addParking()
```

을 사용합니다.

---

# 7. 데이터 담당 구현 가이드

## 담당 파일

```text
src/data_loader.h
src/data_loader.cpp
data/
```

## 목표

서울 데이터 허브에서 가져온 공영주차장 데이터를 읽고 각각의 데이터를 `Parking` 객체로 변환한 후 `ParkingList`에 저장합니다.

## 주요 작업

1. 서울시 공영주차장 데이터 컬럼 분석
2. 프로젝트에서 사용할 필드 선정
3. 데이터 파일 읽기
4. 각 행을 `Parking` 객체로 변환
5. `addParking()`을 이용하여 `ParkingList`에 삽입
6. 잘못된 데이터 또는 누락된 값 처리

## 기본 인터페이스

```cpp
bool loadParkingData(
    const std::string& filename,
    ParkingList* list
);
```

예상 사용:

```cpp
ParkingList parkingLots;

createParkingList(&parkingLots);

if (!loadParkingData(
        "data/parking.csv",
        &parkingLots))
{
    // 데이터 로딩 실패 처리
}
```

## 데이터 처리 흐름

```text
CSV 한 행 읽기
      ↓
Parking 객체 생성
      ↓
필드 데이터 저장
      ↓
addParking()
      ↓
ParkingList 마지막에 삽입
```

## 반환 조건

성공:

```text
true 반환
ParkingList에 데이터 저장 완료
```

실패:

```text
false 반환

예:
- 파일을 열 수 없음
- 데이터 형식 오류
- 처리할 수 없는 필수 필드 오류
```

## 완료 기준

- 데이터 파일 정상 로딩
- 여러 개의 `Parking` 객체 생성 가능
- `ParkingList`에 데이터 정상 삽입
- `head`, `tail`, `size` 정상 갱신
- 누락 데이터로 인해 프로그램이 비정상 종료되지 않음
- 테스트 데이터로 결과 확인

---

# 8. 검색 담당 구현 가이드

## 담당 파일

```text
src/search.h
src/search.cpp
```

## 목표

전체 `ParkingList` 데이터를 대상으로 자치구 및 주차장명 검색 기능을 구현합니다.

검색 속도를 향상하기 위해 Hash Table을 사용합니다.

---

## 기능 1. 검색 인덱스 구축

전체 `ParkingList`를 한 번 순회하면서 자치구 또는 주차장명을 Key로 하는 Hash Table을 구축합니다.

예:

```text
ParkingList

head
 ↓
[A / 광진구]
      ↓
[B / 강남구]
      ↓
[C / 강남구]
      ↓
[D / 광진구]
      ↓
nullptr
```

자치구 Hash Table은 개념적으로 다음과 같이 구성할 수 있습니다.

```text
광진구
  ├─ A 노드 참조
  └─ D 노드 참조

강남구
  ├─ B 노드 참조
  └─ C 노드 참조
```

Hash Table에는 `Parking` 데이터를 다시 복사하기보다는 가능하면 원본 `ParkingNode*`를 참조하도록 구현합니다.

개념적인 참조 노드:

```cpp
struct ParkingRefNode
{
    ParkingNode* parking;
    ParkingRefNode* next;
};
```

이 구조체는 검색 담당자가 실제 Hash Table 구현 방식에 맞게 변경할 수 있습니다.

---

## 기능 2. 자치구 검색

입력:

```text
광진구
```

결과:

```text
광진구에 속하는 Parking 데이터 목록
```

검색 결과는 별도의 `ParkingList`에 복사하여 반환하는 방식을 권장합니다.

예상 인터페이스:

```cpp
bool searchByDistrict(
    const std::string& district,
    ParkingList* result
);
```

사용 예:

```cpp
ParkingList result;

createParkingList(&result);

if (searchByDistrict(
        "광진구",
        &result))
{
    // 검색 결과 사용
}
```

검색 결과가 여러 개라면:

```text
result

[광진구 A주차장]
        ↓
[광진구 B주차장]
        ↓
[광진구 C주차장]
        ↓
nullptr
```

와 같이 생성합니다.

---

## 기능 3. 주차장명 검색

예상 인터페이스:

```cpp
bool searchByName(
    const std::string& name,
    ParkingList* result
);
```

초기 구현에서는 **정확한 주차장명 검색**을 우선합니다.

예:

```text
입력

광진구 공영주차장

→ 이름이 정확히 일치하는 데이터 검색
```

부분 문자열 검색은 필요하면 추후 확장합니다.

---

## 검색 시 주의사항

Hash Table 자체에는 원본 `ParkingNode*`를 저장해도 되지만,

사용자가 검색한 결과에 정렬이나 추천 기능을 적용할 예정이므로 검색 결과는 별도의 `ParkingList`로 복사하는 방법을 권장합니다.

즉:

```text
원본 ParkingList
       ↓
Hash Table
       ↓
검색
       ↓
검색 결과 ParkingList 생성
       ↓
정렬 / 추천
```

형태로 사용합니다.

그러면 검색 결과를 정렬해도 원본 데이터나 Hash Table의 참조 관계가 변경되지 않습니다.

---

## 완료 기준

- Hash Table 구축 가능
- 자치구 검색 가능
- 주차장명 검색 가능
- 존재하지 않는 검색어 처리 가능
- 동일 자치구에 여러 주차장이 있어도 모두 반환
- 검색 결과를 `ParkingList`로 전달 가능
- Hash Table 평균 검색 시간복잡도 설명 가능

Hash Table의 검색은 평균적으로 O(1)을 기대하지만, 충돌이 심한 경우 최악의 성능은 더 나빠질 수 있음을 설명할 수 있어야 합니다.

---

# 9. 정렬 담당 구현 가이드

## 담당 파일

```text
src/sort.h
src/sort.cpp
```

## 목표

검색 결과로 생성된 `ParkingList`를 사용자가 원하는 조건으로 정렬합니다.

Linked List에 적합한 **Merge Sort**를 우선 사용합니다.

---

## 기능 1. 요금 기준 정렬

예상 인터페이스:

```cpp
void sortByFee(
    ParkingList* list
);
```

1시간 요금(`hourlyFee`)이 낮은 순서부터 정렬합니다. 주차장마다 기본 시간(5분, 30분 등)이 달라 기본 요금끼리는 비교할 수 없기 때문입니다. 요금 정보가 없는 주차장(`FEE_UNKNOWN`)은 맨 뒤로 보냅니다.

```text
1000원
 ↓
2000원
 ↓
3000원
 ↓
5000원
```

---

## 기능 2. 주차면수 기준 정렬

예상 인터페이스:

```cpp
void sortByCapacity(
    ParkingList* list
);
```

주차면수가 많은 순으로 정렬하는 것을 기본 규칙으로 합니다.

```text
300면
 ↓
200면
 ↓
100면
 ↓
50면
```

---

## Merge Sort 사용 이유

Linked List는 배열처럼 임의 위치 접근이 편리하지 않습니다.

반면 Merge Sort는 리스트를 두 부분으로 분리한 뒤 정렬된 리스트를 다시 연결하는 방식이므로 Linked List와 잘 맞습니다.

```text
A → B → C → D → E → F

        ↓ 분할

A → B → C

D → E → F

        ↓ 각각 정렬

        ↓ Merge

정렬된 Linked List
```

예상 시간복잡도:

```text
O(m log m)
```

- `m`: 정렬 대상 검색 결과의 개수

---

## 정렬 시 주의사항

가능하면 `Parking` 데이터 자체를 다른 노드와 교환하기보다 `next` 연결을 재구성하는 방식의 Merge Sort를 사용합니다.

특히 원본 데이터와 검색 인덱스의 참조 관계를 유지하기 위해 원본 `ParkingList`를 직접 정렬하기보다는 검색 결과용 `ParkingList`를 정렬하는 방식을 권장합니다.

---

## 완료 기준

- 요금 오름차순 정렬 정상 동작
- 주차면수 내림차순 정렬 정상 동작
- 빈 리스트 처리
- 노드가 하나뿐인 경우 처리
- 동일한 값이 여러 개 존재하는 경우 처리
- `head`와 `tail` 정상 유지
- Merge Sort 시간복잡도 설명 가능

---

# 10. 추천 담당 구현 가이드

## 담당 파일

```text
src/recommendation.h
src/recommendation.cpp
```

## 목표

검색 결과 중 1시간 요금(`hourlyFee`)이 가장 저렴한 Top-N 주차장을 Heap을 이용하여 선택합니다. 요금 정보가 없는 주차장은 추천에서 제외합니다.

예:

```text
광진구에서
가장 저렴한 주차장 5개 추천
```

---

## 기본 인터페이스

```cpp
bool recommendTopNByFee(
    const ParkingList* candidates,
    ParkingList* result,
    int topN
);
```

여기서:

```text
candidates
→ 추천 후보 주차장 목록

result
→ 최종 추천 결과

topN
→ 사용자에게 추천할 주차장 개수
```

입니다.

`topN`은 전체 데이터 개수가 아니라 기능 자체에 필요한 값이므로 별도 매개변수로 전달합니다.

---

## 예상 사용

```cpp
ParkingList recommendation;

createParkingList(&recommendation);

recommendTopNByFee(
    &searchResult,
    &recommendation,
    5
);
```

예상 결과:

```text
recommendation

[1000원 주차장]
        ↓
[1500원 주차장]
        ↓
[2000원 주차장]
        ↓
...
```

---

## Heap 구현

Heap은 배열 기반으로 직접 구현하는 방식을 고려할 수 있습니다.

예:

```cpp
struct ParkingHeap
{
    ParkingNode** data;
    int size;
    int capacity;
};
```

`std::vector`를 사용하지 않습니다.

추천 후보를 순회하면서 Heap 크기를 최대 `topN`으로 유지하는 방식을 사용하면

```text
O(m log k)
```

시간복잡도를 목표로 할 수 있습니다.

- `m`: 추천 후보 개수
- `k`: 추천할 주차장 개수

---

## 완료 기준

- Top-N 추천 정상 동작
- `topN`이 1인 경우 처리
- `topN`이 후보 개수보다 큰 경우 처리
- 빈 리스트 처리
- 동일 요금 데이터 처리
- Heap 크기 정상 관리
- 사용한 Heap 연산과 시간복잡도 설명 가능

---

# 11. 발표 / 테스트 담당 구현 가이드

## 담당 영역

```text
tests/
README.md
docs/
발표자료
```

별도의 UI 모듈은 구현하지 않습니다.

프로그램 입력과 출력은 `main.cpp`에서 콘솔 기반으로 처리합니다.

---

## 주요 역할

1. 각 기능 정상 동작 테스트
2. 예외 상황 테스트
3. 실제 실행 결과 확인
4. README 최신화
5. 자료구조 사용 이유 정리
6. 시간복잡도 정리
7. GitHub 개발 과정 정리
8. 실행 화면 준비
9. 발표자료 작성
10. 최종 발표

---

## Linked List 테스트

```text
빈 리스트 생성
→ head = nullptr
→ tail = nullptr
→ size = 0

첫 데이터 삽입
→ head == tail

여러 데이터 삽입
→ tail 정상 변경

destroyParkingList()
→ 모든 노드 해제
```

---

## 데이터 로딩 테스트

```text
정상 데이터 입력
→ ParkingList 정상 생성

파일 없음
→ 오류 처리

필드 누락
→ 프로그램 비정상 종료 없이 처리
```

---

## 검색 테스트

```text
광진구 검색
→ 광진구 주차장 목록 정상 생성

존재하지 않는 자치구
→ 검색 결과 없음

존재하는 주차장명
→ 정상 검색

존재하지 않는 주차장명
→ 검색 결과 없음
```

---

## 정렬 테스트

```text
요금순 정렬
→ 낮은 요금부터 출력

주차면수순 정렬
→ 많은 주차면수부터 출력

빈 리스트
→ 오류 없이 처리

동일 요금
→ 오류 없이 처리
```

---

## 추천 테스트

```text
Top-5 요청
→ 최대 5개 결과 생성

후보 3개 + Top-5 요청
→ 3개만 반환

후보 없음
→ 결과 없음 처리
```

---

## 출력 결과 검증

콘솔에서 다음과 같이 확인할 수 있어야 합니다.

```text
--------------------------------
주차장명 : 광진구 공영주차장
주소     : 서울특별시 광진구 ...
자치구   : 광진구
주차면수 : 120
1시간 요금 : 1000원
운영시간 : 09:00 ~ 22:00
--------------------------------
```

---

## 발표 시 정리해야 할 내용

- 프로그램 전체 데이터 흐름
- Linked List를 사용한 이유
- Hash Table을 사용한 이유
- Merge Sort를 사용한 이유
- Heap을 사용한 이유
- 주요 함수 역할
- 시간복잡도
- 실행 결과
- GitHub Issue / Branch / PR 활용 과정
- 구현 중 발생한 문제와 해결 과정

---

# 12. 조장 / 통합 담당 가이드

## 담당 파일

```text
src/main.cpp
src/parking.h
src/parking_list.h
src/parking_list.cpp
```

및 전체 프로젝트 구조를 관리합니다.

## 주요 역할

1. `Parking` 구조 설계
2. `ParkingList` 구조 설계
3. Linked List 기본 함수 구현
4. 각 모듈 함수 인터페이스 협의 및 확정
5. `main.cpp` 작성
6. 사용자 입력 및 결과 출력
7. 팀원 모듈 통합
8. Pull Request 확인
9. Conflict 관리
10. 최종 빌드 및 테스트

---

# 13. main.cpp의 역할

`main.cpp`에서는 Hash Table, Merge Sort, Heap 등의 세부 알고리즘을 직접 구현하지 않습니다.

각 모듈의 함수를 연결하는 역할만 담당합니다.

기본 구조 예:

```cpp
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
        std::cout << "데이터 로딩 실패\n";
        return 1;
    }

    int menu;

    while (true)
    {
        std::cout << "\n";
        std::cout
            << "===== 서울시 공영주차장 안내 프로그램 =====\n";

        std::cout << "1. 전체 주차장 조회\n";
        std::cout << "2. 자치구 검색\n";
        std::cout << "3. 주차장명 검색\n";
        std::cout << "4. 요금순 정렬\n";
        std::cout << "5. 주차면수순 정렬\n";
        std::cout << "6. 요금 기준 Top-N 추천\n";
        std::cout << "0. 종료\n";
        std::cout << "선택: ";

        std::cin >> menu;

        if (menu == 0)
            break;

        // 메뉴에 따라
        // search / sort / recommendation 함수 호출
        // 최종 결과 콘솔 출력
    }

    destroyParkingList(&parkingLots);

    return 0;
}
```

위 코드는 전체 흐름을 보여주는 기본 예시입니다.

실제 검색용 Hash Table 초기화 과정 등은 검색 담당과 인터페이스 확정 후 추가합니다.

---

# 14. 모듈 간 구현 원칙

각 모듈은 자신의 역할만 수행합니다.

## 잘못된 형태

```cpp
// search.cpp에서

// 데이터 파일 읽기
// Hash Table 생성
// 검색
// 정렬
// cout으로 출력
```

## 권장 형태

```text
parking_list
→ Linked List 기본 관리

data_loader
→ 데이터 읽기
→ Parking 객체 생성
→ ParkingList 삽입

search
→ Hash Table 생성
→ 검색
→ 결과 ParkingList 생성

sort
→ 결과 ParkingList 정렬

recommendation
→ Heap 기반 Top-N 결과 생성

main
→ 사용자 입력
→ 각 모듈 함수 호출
→ 결과 출력
```

`search.cpp`, `sort.cpp`, `recommendation.cpp`에서는 직접 사용자 입력을 받거나 메뉴를 출력하지 않습니다.

---

# 15. 함수 인터페이스 변경 규칙

다른 모듈에서 사용하는 함수 이름 또는 매개변수는 임의로 변경하지 않습니다.

예:

```cpp
searchByDistrict(...)
```

를 검색 담당자가 혼자

```cpp
findDistrictParkingData(...)
```

로 변경하면 다른 모듈과 연결되지 않을 수 있습니다.

변경이 필요한 경우:

```text
변경 필요 발견
      ↓
Issue 또는 팀 채팅 공유
      ↓
관련 담당자 확인
      ↓
조장 확인
      ↓
인터페이스 변경
      ↓
관련 모듈 수정
```

순서로 진행합니다.

---

# 16. Linked List 사용 공통 규칙

전체 `Parking` 데이터는 `ParkingList`를 이용하여 관리합니다.

프로젝트의 전체 데이터 저장이나 검색 결과 저장을 위해 `std::vector`를 사용하지 않습니다.

노드 추가 시 가능한 한 공통 함수:

```cpp
addParking()
```

을 사용합니다.

Linked List 사용이 끝난 경우 반드시:

```cpp
destroyParkingList()
```

또는

```cpp
clearParkingList()
```

를 호출하여 동적으로 생성한 노드를 해제합니다.

공통 `ParkingList`, `ParkingNode` 구조체를 담당자 협의 없이 변경하지 않습니다.

---

# 17. 자료구조 활용 계획

현재 프로젝트에서 활용하는 주요 자료구조와 알고리즘은 다음과 같습니다.

```text
Linked List
→ 전체 Parking 데이터 저장
→ 검색 결과 저장

Hash Table
→ 자치구 / 주차장명 검색

Merge Sort
→ 요금 / 주차면수 정렬

Heap
→ 요금 기준 Top-N 추천
```

모든 담당자는 단순히 코드가 동작하는 것뿐만 아니라 다음 내용을 설명할 수 있어야 합니다.

```text
왜 이 자료구조를 선택했는가?

어떤 연산이 수행되는가?

시간복잡도는 얼마인가?

다른 자료구조와 비교했을 때 어떤 장점이 있는가?
```

---

# 18. STL 사용 관련 주의사항

프로젝트에서 `std::vector`는 사용하지 않습니다.

Hash Table, Heap, Sorting과 관련된 STL:

```cpp
std::unordered_map
std::priority_queue
std::sort
```

의 사용 여부는 교수님의 평가 기준을 확인한 뒤 팀 전체에서 동일한 기준을 적용합니다.

자료구조 자체의 구현 과정이 평가 대상이라면 직접 구현하는 방식을 우선합니다.

---

# 19. 메모리 관리 주의사항

Linked List와 Heap에서 동적 메모리를 사용하므로 메모리 해제를 반드시 확인합니다.

예:

```cpp
ParkingNode* node = new ParkingNode;
```

를 사용했다면 해당 노드는 최종적으로:

```cpp
delete node;
```

되어야 합니다.

프로그램 종료 전에:

```cpp
destroyParkingList(&parkingLots);
```

를 호출합니다.

검색 결과나 추천 결과로 별도의 `ParkingList`를 만들었다면 해당 리스트 역시 작업 완료 후 해제해야 합니다.

예:

```cpp
ParkingList result;

createParkingList(&result);

searchByDistrict(
    "광진구",
    &result
);

// 결과 사용

destroyParkingList(&result);
```

---

# 20. GitHub 작업 방법

모든 작업은 Issue 단위로 진행합니다.

```text
Issue 확인
    ↓
main 최신화
    ↓
작업 브랜치 생성
    ↓
구현
    ↓
테스트
    ↓
Commit
    ↓
Push
    ↓
Pull Request
    ↓
Review
    ↓
Squash and Merge
```

브랜치 예:

```text
feature/3-parking-list
feature/4-data-loader
feature/5-district-search
feature/6-name-search
feature/7-fee-sort
feature/8-capacity-sort
feature/9-topn-recommendation
test/10-integration-test
docs/11-readme
```

`main` 브랜치에서 직접 개발하지 않습니다.

---

# 21. Pull Request 전 확인 사항

PR을 생성하기 전에 다음 내용을 확인합니다.

- [ ] 담당 Issue의 기능을 모두 구현했는가?
- [ ] 정상적으로 컴파일되는가?
- [ ] 정상 입력을 테스트했는가?
- [ ] 예외 입력을 테스트했는가?
- [ ] 불필요한 파일을 Commit하지 않았는가?
- [ ] `std::vector`를 사용하지 않았는가?
- [ ] 공통 `Parking` 구조를 임의로 변경하지 않았는가?
- [ ] 공통 `ParkingList` 구조를 임의로 변경하지 않았는가?
- [ ] Linked List의 `head`, `tail`, `size`가 정상적으로 관리되는가?
- [ ] 동적으로 생성한 메모리가 정상적으로 해제되는가?
- [ ] 함수 및 주요 로직에 필요한 주석이 있는가?
- [ ] 사용한 자료구조의 선택 이유를 설명할 수 있는가?
- [ ] 주요 연산의 시간복잡도를 설명할 수 있는가?

---

# 22. 구현 완료 기준

단순히 코드를 작성했다고 완료된 것으로 판단하지 않습니다.

다음 조건을 모두 만족해야 해당 기능 구현이 완료된 것으로 간주합니다.

```text
코드 작성
+
컴파일 성공
+
정상 입력 테스트
+
예외 입력 테스트
+
메모리 관리 확인
+
다른 모듈과 연결 가능
+
시간복잡도 확인
+
PR 생성
+
Code Review
```

---

# 23. 최종 프로그램 데이터 흐름

최종적으로 프로그램은 다음 흐름으로 동작하도록 구현합니다.

```text
서울시 공영주차장 데이터
          ↓
      Data Loader
          ↓
       Parking
          ↓
     ParkingList
    (Linked List)
          ↓
      Hash Table
          ↓
   사용자 검색 조건
          ↓
  검색 결과 ParkingList
          ↓
 ┌────────┴────────┐
 ↓                 ↓
Merge Sort        Heap
 ↓                 ↓
정렬 결과       Top-N 추천
 └────────┬────────┘
          ↓
       main.cpp
          ↓
     Console Output
```

---

# 24. 공통 목표

각 팀원이 자신의 기능만 개별적으로 완성하는 것이 최종 목표가 아닙니다.

최종적으로:

```text
Data Loader
     ↓
Linked List
     ↓
Hash Table Search
     ↓
Search Result List
     ↓
Merge Sort / Heap
     ↓
main.cpp
     ↓
Console Output
```

의 전체 흐름이 하나의 프로그램에서 정상적으로 동작해야 합니다.

기능이나 공통 구조를 변경해야 하는 경우 혼자 결정하지 않고 관련 담당자와 먼저 공유합니다.

최종적으로 모든 팀원이 자신이 구현한 기능뿐 아니라:

- 전체 데이터 흐름
- Linked List의 역할
- Hash Table의 역할
- Merge Sort의 역할
- Heap의 역할
- 주요 연산의 시간복잡도

를 설명할 수 있도록 합니다.
