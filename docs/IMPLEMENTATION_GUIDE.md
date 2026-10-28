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
검색 자료구조 구축
        ↓
사용자 조건 입력
        ↓
검색
        ↓
정렬 / 추천
        ↓
콘솔 결과 출력
```

각 담당자는 위 과정 중 자신의 부분을 모듈 단위로 구현합니다.

프로그램의 메뉴 선택과 최종 결과 출력은 `main.cpp`에서 처리합니다.

---

# 2. 담당 역할

| 역할 | 주요 담당 |
|---|---|
| 조장 / 통합 | 공통 구조 설계, 인터페이스 정의, `main.cpp`, 전체 모듈 통합, GitHub 관리 |
| 데이터 | 서울시 공영주차장 데이터 분석, 데이터 로딩 및 Vector 저장 |
| 검색 | 자치구 / 주차장명 검색 및 Hash Table 기반 검색 자료구조 구현 |
| 정렬 / 추천 | 정렬 알고리즘 및 Heap 기반 Top-N 추천 구현 |
| 발표 / 테스트 | 기능 테스트, 예외 상황 검증, README 정리, 발표자료 작성 및 최종 발표 |

---

# 3. 공통 데이터 구조

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
    int basicFee;           // 기본 요금

    std::string openTime;   // 운영 시작 시간
    std::string closeTime;  // 운영 종료 시간
};

#endif
```

※ 위 필드는 서울 데이터 허브의 실제 데이터 컬럼 분석 후 최종 확정합니다.

## 공통 규칙

`Parking` 구조체는 다른 팀원이 임의로 수정하지 않습니다.

새로운 필드가 필요한 경우 Issue 또는 팀 회의를 통해 먼저 공유한 뒤 변경합니다.

예:

```text
parking.h에 weekendFee 추가 필요
        ↓
Issue / 팀 채팅 공유
        ↓
조장 + 데이터 담당 확인
        ↓
공통 구조 수정
        ↓
다른 팀원 코드 반영
```

---

# 4. 프로젝트 파일 구조

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
│   ├── parking.h
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
├── tests/
└── docs/
    └── IMPLEMENTATION_GUIDE.md
```

각 모듈의 역할은 다음과 같습니다.

```text
main.cpp
→ 프로그램 실행 흐름
→ 사용자 입력
→ 메뉴 처리
→ 각 모듈 호출
→ 결과 출력

parking.h
→ 공통 Parking 구조체

data_loader
→ 데이터 로딩

search
→ 검색

sort
→ 정렬

recommendation
→ Top-N 추천

tests
→ 기능 검증
```

---

# 5. 데이터 담당 구현 가이드

## 담당 파일

```text
src/data_loader.h
src/data_loader.cpp
data/
```

## 목표

서울 데이터 허브에서 가져온 공영주차장 데이터를 프로그램에서 사용할 수 있는 `Parking` 객체로 변환합니다.

## 주요 작업

1. 서울시 공영주차장 데이터 컬럼 분석
2. 프로젝트에서 사용할 필드 선정
3. 데이터 파일 읽기
4. 각 행을 `Parking` 객체로 변환
5. 전체 데이터를 `std::vector<Parking>` 형태로 반환
6. 잘못된 데이터 또는 누락된 값 처리

## 기본 인터페이스

```cpp
std::vector<Parking> loadParkingData(
    const std::string& filename
);
```

예상 사용:

```cpp
std::vector<Parking> parkingLots =
    loadParkingData("data/parking.csv");
```

Vector의 데이터 개수는 별도의 `n` 변수로 전달하지 않고 필요한 경우

```cpp
parkingLots.size()
```

를 사용합니다.

## 반환 조건

성공:

```text
Parking 객체가 들어 있는 vector 반환
```

실패:

```text
파일을 열 수 없음
잘못된 데이터 형식
필수 필드 누락
```

등의 상황을 적절히 처리합니다.

## 완료 기준

- 데이터 파일 정상 로딩
- 여러 개의 `Parking` 객체 생성 가능
- 주요 필드 값 정상 저장
- 누락 데이터로 프로그램이 비정상 종료되지 않음
- 간단한 테스트 데이터로 결과 확인
- 다른 모듈에서 반환된 Vector 사용 가능

---

# 6. 검색 담당 구현 가이드

## 담당 파일

```text
src/search.h
src/search.cpp
```

## 목표

전체 주차장 데이터를 대상으로 자치구 및 주차장명 검색 기능을 구현합니다.

검색 효율 향상을 위해 Hash Table을 사용합니다.

검색 자료구조에는 `Parking` 객체를 중복 저장하기보다 원본 Vector의 index를 저장하는 방식을 우선 사용합니다.

---

## 기능 1. 검색 인덱스 구축

전체 주차장 데이터를 한 번 순회하면서 자치구 또는 주차장명을 Key로 하는 Hash Table을 구축합니다.

예:

```text
parkingLots

index
0 → A주차장 / 광진구
1 → B주차장 / 강남구
2 → C주차장 / 강남구
3 → D주차장 / 광진구
```

자치구 Hash Table:

```text
광진구 → [0, 3]
강남구 → [1, 2]
```

이렇게 하면 동일한 `Parking` 객체를 다시 저장하지 않고 기존 Vector를 참조할 수 있습니다.

---

## 기능 2. 자치구 검색

입력:

```text
광진구
```

출력:

```text
광진구에 속하는 Parking 데이터 목록
```

예상 인터페이스의 개념:

```cpp
std::vector<int> searchByDistrict(
    const std::string& district
);
```

반환되는 `int` 값은 원본 `parkingLots` Vector의 index를 의미합니다.

예:

```text
광진구 검색 결과
→ [0, 3]
```

`main.cpp`에서는 해당 index를 이용하여 실제 주차장 정보를 출력합니다.

---

## 기능 3. 주차장명 검색

예상 인터페이스:

```cpp
std::vector<int> searchByName(
    const std::string& name
);
```

초기 구현에서는 정확한 주차장명 검색을 우선합니다.

예:

```text
입력:
광진구 공영주차장

→ 해당 이름과 일치하는 주차장 반환
```

부분 문자열 검색은 필요할 경우 추후 확장 기능으로 구현합니다.

---

## 완료 기준

- 검색용 Hash Table 구축 가능
- 자치구 검색 가능
- 주차장명 검색 가능
- 존재하지 않는 검색어 처리 가능
- 동일 자치구에 여러 주차장이 있어도 모두 반환
- 원본 Vector index를 통해 데이터 접근 가능
- 평균 검색 시간복잡도 설명 가능

Hash Table의 검색은 일반적으로 평균적으로 O(1)이지만, 충돌 상황에 따라 최악의 경우 성능이 저하될 수 있음을 설명할 수 있어야 합니다.

---

# 7. 정렬 / 추천 담당 구현 가이드

## 담당 파일

```text
src/sort.h
src/sort.cpp

src/recommendation.h
src/recommendation.cpp
```

---

## 기능 1. 요금 기준 정렬

예상 인터페이스:

```cpp
void sortByFee(
    std::vector<Parking>& parkingLots
);
```

기본 요금이 낮은 순서부터 정렬합니다.

예:

```text
1000원
2000원
3000원
5000원
```

Vector의 개수를 나타내는 `n`을 별도 매개변수로 전달하지 않습니다.

잘못된 예:

```cpp
void sortByFee(
    std::vector<Parking>& parkingLots,
    int n
);
```

권장 방식:

```cpp
parkingLots.size()
```

를 사용합니다.

---

## 기능 2. 주차면수 기준 정렬

예상 인터페이스:

```cpp
void sortByCapacity(
    std::vector<Parking>& parkingLots
);
```

주차면수가 많은 순으로 정렬하는 것을 기본 규칙으로 합니다.

예:

```text
300면
200면
100면
50면
```

필요한 경우 오름차순 / 내림차순 옵션을 추가할 수 있으나 초기 구현에서는 한 가지 규칙으로 통일합니다.

---

## 기능 3. Top-N 추천

예:

```text
광진구에서 요금이 가장 저렴한 주차장 5개
```

예상 인터페이스:

```cpp
std::vector<Parking> recommendTopNByFee(
    const std::vector<Parking>& parkingLots,
    int topN
);
```

여기서 `topN`은 Vector의 크기가 아니라 실제로 사용자에게 추천할 데이터의 개수입니다.

예:

```text
전체 검색 결과 개수 = 50개
topN = 5

→ 가장 저렴한 주차장 5개 반환
```

따라서 `topN`은 필요한 매개변수입니다.

Heap / Priority Queue를 사용하여 Top-N 데이터를 선택합니다.

제안서에서 목표로 한 시간복잡도는

```text
O(m log k)
```

입니다.

- `m`: 추천 후보 주차장 개수
- `k`: 추천할 주차장 개수 (`topN`)

따라서 전체 데이터를 정렬한 뒤 앞의 N개만 가져오는 방식보다, 크기 `k`의 Heap을 유지하는 방식의 구현을 우선 고려합니다.

---

## 완료 기준

- 요금 기준 정렬 정상 동작
- 주차면수 기준 정렬 정상 동작
- 동일한 값이 여러 개 존재하는 경우 처리
- 데이터가 없는 경우 처리
- `topN`이 데이터 개수보다 큰 경우 처리
- 정렬 알고리즘의 시간복잡도 설명 가능
- Heap 기반 Top-N 추천의 시간복잡도 설명 가능

---

# 8. 발표 / 테스트 담당 구현 가이드

## 담당 파일 및 영역

```text
tests/
README.md
docs/
발표자료
```

별도의 UI 모듈은 구현하지 않습니다.

프로그램의 입력과 출력은 `main.cpp`에서 콘솔 기반으로 처리합니다.

## 주요 역할

1. 각 기능의 정상 동작 테스트
2. 예외 상황 테스트
3. 실제 실행 결과 확인
4. README 최신화
5. 프로그램 구조 정리
6. 자료구조 사용 이유 정리
7. 시간복잡도 정리
8. GitHub 개발 과정 정리
9. 실행 결과 화면 준비
10. 발표자료 작성
11. 최종 발표

---

## 테스트 항목

### 데이터 로딩 테스트

```text
정상 CSV 입력
→ 모든 Parking 데이터 정상 생성

파일 없음
→ 오류 처리

일부 필드 누락
→ 프로그램 비정상 종료 없이 처리
```

---

### 검색 테스트

```text
광진구 검색
→ 광진구 주차장 목록 정상 반환

존재하지 않는 자치구 검색
→ 검색 결과 없음 처리

존재하는 주차장명 검색
→ 정상 결과 반환

존재하지 않는 주차장명 검색
→ 검색 결과 없음 처리
```

---

### 정렬 테스트

```text
요금순 정렬
→ 낮은 요금부터 정상 정렬

주차면수 정렬
→ 많은 주차면수부터 정상 정렬

동일한 요금이 존재하는 경우
→ 오류 없이 처리
```

---

### 추천 테스트

```text
Top-5 추천
→ 5개 결과 반환

검색 결과 3개 + Top-5 요청
→ 존재하는 3개만 반환

검색 결과 없음
→ 추천 결과 없음 처리
```

---

## 출력 결과 검증

별도의 UI는 구현하지 않지만 콘솔에서 사용자가 결과를 확인할 수 있어야 합니다.

예:

```text
--------------------------------
주차장명 : 광진구 공영주차장
주소     : 서울특별시 광진구 ...
자치구   : 광진구
주차면수 : 120
기본요금 : 1000원
운영시간 : 09:00 ~ 22:00
--------------------------------
```

---

## 발표 시 정리해야 할 내용

- 프로그램 전체 처리 흐름
- Vector 사용 이유
- Hash Table 사용 이유
- Sorting 사용 이유
- Heap 사용 이유
- 주요 함수 역할
- 각 연산의 시간복잡도
- 실제 프로그램 실행 결과
- GitHub Issue / Branch / PR 활용 과정
- 프로젝트 구현 중 발생한 문제와 해결 방법

---

# 9. 조장 / 통합 담당

## 담당 파일

```text
src/main.cpp
src/parking.h
```

및 전체 프로젝트 구조를 관리합니다.

## 주요 역할

1. 공통 `Parking` 구조 설계
2. 함수 인터페이스 협의 및 확정
3. 담당 모듈 간 연결
4. `main.cpp` 작성
5. 사용자 입력 및 결과 출력 처리
6. Pull Request 확인
7. Conflict 관리
8. 전체 프로그램 통합
9. 최종 빌드 및 테스트

---

## main.cpp의 역할

`main.cpp`에서는 세부 자료구조 알고리즘을 직접 구현하지 않습니다.

각 담당 모듈에서 구현한 함수를 호출하고 전체 프로그램 흐름을 연결합니다.

예:

```cpp
#include <iostream>
#include <vector>

#include "parking.h"
#include "data_loader.h"
#include "search.h"
#include "sort.h"
#include "recommendation.h"

int main()
{
    std::vector<Parking> parkingLots =
        loadParkingData("data/parking.csv");

    int menu;

    while (true)
    {
        std::cout << "\n";
        std::cout << "===== 서울시 공영주차장 안내 프로그램 =====\n";
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
        // 결과를 콘솔에 출력
    }

    return 0;
}
```

위 코드는 프로그램 구조를 보여주는 예시이며 실제 함수 이름과 세부 처리 방식은 최종 인터페이스에 맞춰 수정합니다.

---

# 10. 모듈 간 구현 원칙

각 기능은 최대한 독립적으로 구현합니다.

잘못된 형태:

```cpp
// search.cpp에서

// 파일을 직접 읽음
// 검색 수행
// 정렬 수행
// cout으로 결과까지 출력
```

권장 형태:

```text
data_loader
→ 데이터만 읽고 Parking Vector 생성

search
→ 검색만 수행하고 결과 반환

sort
→ 정렬만 수행

recommendation
→ 추천 결과만 생성

main
→ 사용자 입력
→ 각 기능 호출
→ 최종 결과 출력
```

즉 하나의 모듈이 다른 담당자의 기능까지 모두 처리하지 않습니다.

특히 `search.cpp`, `sort.cpp`, `recommendation.cpp`에서는 가능하면 직접 메뉴를 출력하거나 사용자 입력을 받지 않습니다.

---

# 11. 함수 인터페이스 변경 규칙

다른 모듈에서 사용하는 함수 이름 또는 매개변수를 임의로 변경하면 안 됩니다.

예:

```cpp
searchByDistrict(...)
```

를 검색 담당자가 임의로

```cpp
findDistrictParkingData(...)
```

로 변경하면 다른 팀원의 코드가 컴파일되지 않을 수 있습니다.

인터페이스 변경이 필요한 경우:

```text
변경 필요 발견
    ↓
Issue 또는 팀 채팅 공유
    ↓
관련 담당자 확인
    ↓
조장 확인
    ↓
함수 인터페이스 변경
    ↓
관련 모듈 수정
```

순으로 진행합니다.

---

# 12. Vector 사용 관련 공통 규칙

Vector는 자신의 데이터 개수를 `size()`를 통해 관리할 수 있으므로, Vector의 크기만을 의미하는 `n`은 별도의 매개변수로 전달하지 않습니다.

잘못된 예:

```cpp
void sortByFee(
    std::vector<Parking>& parkingLots,
    int n
);
```

권장 예:

```cpp
void sortByFee(
    std::vector<Parking>& parkingLots
);
```

함수 내부에서는:

```cpp
parkingLots.size()
```

를 사용합니다.

단, 다음과 같이 기능 자체에 필요한 값은 별도의 매개변수로 사용합니다.

```cpp
recommendTopNByFee(parkingLots, topN);
```

여기서 `topN`은 Vector 크기가 아니라 추천할 주차장 개수를 의미하므로 필요한 값입니다.

---

# 13. 자료구조 구현 관련 주의사항

본 프로젝트는 자료구조 과목 프로젝트이므로 단순히 프로그램이 동작하는 것뿐만 아니라 다음 내용을 설명할 수 있어야 합니다.

```text
왜 이 자료구조를 선택했는가?

어떤 연산을 수행하는가?

시간복잡도는 얼마인가?

다른 자료구조와 비교했을 때 어떤 장점이 있는가?
```

현재 프로젝트에서 사용하는 주요 자료구조 및 개념은 다음과 같습니다.

```text
Vector
→ 전체 Parking 데이터 저장

Hash Table
→ 자치구 / 주차장명 검색

Sorting
→ 요금 / 주차면수 정렬

Heap
→ Top-N 주차장 추천
```

STL 사용 여부와 직접 구현 범위는 교수님의 평가 기준에 따라 결정합니다.

예:

```cpp
std::unordered_map
std::priority_queue
std::sort
```

를 그대로 사용할지,

Hash Table / Heap / Sorting 알고리즘 자체를 구현할지는 팀 전체가 동일한 기준을 적용합니다.

자료구조 구현 자체가 평가 대상이라면 직접 구현하는 방향을 우선 고려합니다.

---

# 14. GitHub 작업 방법

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
feature/4-data-loader
feature/5-district-search
feature/6-name-search
feature/7-fee-sort
feature/8-capacity-sort
feature/9-topn-recommendation
test/12-integration-test
docs/13-readme
```

`main` 브랜치에서 직접 개발하지 않습니다.

---

# 15. Pull Request 전 확인 사항

PR을 생성하기 전에 다음 내용을 확인합니다.

- [ ] 담당 Issue의 기능을 모두 구현했는가?
- [ ] 정상적으로 컴파일되는가?
- [ ] 기본 기능을 직접 테스트했는가?
- [ ] 예외 상황을 확인했는가?
- [ ] 불필요한 파일을 Commit하지 않았는가?
- [ ] 담당하지 않은 공통 코드를 임의로 변경하지 않았는가?
- [ ] 함수 및 주요 로직에 필요한 주석이 있는가?
- [ ] Vector 크기를 의미하는 불필요한 `n`을 사용하지 않았는가?
- [ ] 사용한 자료구조의 선택 이유를 설명할 수 있는가?
- [ ] 주요 연산의 시간복잡도를 설명할 수 있는가?

---

# 16. 구현 완료 기준

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
다른 모듈과 연결 가능
+
시간복잡도 확인
+
PR 생성
+
Code Review
```

---

# 17. 공통 목표

각자의 기능을 따로 구현하는 것이 최종 목표가 아니라,

```text
Data Loader
     ↓
Vector<Parking>
     ↓
Hash Table Search
     ↓
Sorting / Heap Recommendation
     ↓
main.cpp
     ↓
Console Output
```

의 흐름이 하나의 프로그램으로 정상적으로 연결되는 것이 최종 목표입니다.

기능 또는 공통 구조를 변경해야 하는 경우 혼자 결정하지 않고 관련 담당자와 먼저 공유합니다.

최종적으로 모든 팀원이 자신이 구현한 기능뿐만 아니라 프로그램 전체 데이터 흐름과 사용한 자료구조의 역할을 설명할 수 있도록 합니다.
