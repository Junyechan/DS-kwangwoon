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
전체 데이터 저장
        ↓
검색 자료구조 구축
        ↓
사용자 조건 입력
        ↓
검색
        ↓
정렬 / 추천
        ↓
결과 출력
```

각 담당자는 위 과정 중 자신의 부분을 모듈 단위로 구현합니다.

---

# 2. 담당 역할

| 역할 | 주요 담당 |
|---|---|
| 조장 / 통합 | 공통 구조 설계, 인터페이스 정의, main, 전체 모듈 통합 |
| 데이터 | 공영주차장 데이터 분석 및 로딩 |
| 검색 | 자치구 / 주차장명 검색 및 검색용 자료구조 |
| 정렬 / 추천 | 정렬 알고리즘 및 Top-N 추천 |
| 발표 / UI / 테스트 | 메뉴, 출력, 예외처리, 테스트, README, 발표 |

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

### 공통 규칙

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
│   ├── recommendation.cpp
│   │
│   ├── ui.h
│   └── ui.cpp
│
├── data/
├── tests/
└── docs/
    └── IMPLEMENTATION_GUIDE.md
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
5. 전체 데이터를 `vector<Parking>` 형태로 반환
6. 잘못된 데이터 또는 누락된 값 처리

## 기본 인터페이스

```cpp
std::vector<Parking> loadParkingData(const std::string& filename);
```

예상 사용:

```cpp
std::vector<Parking> parkingLots =
    loadParkingData("data/parking.csv");
```

## 반환 조건

성공:

```text
Parking 객체가 들어 있는 vector 반환
```

실패:

```text
파일을 열 수 없음
잘못된 데이터 형식
```

등의 상황을 적절히 처리합니다.

## 완료 기준

- 데이터 파일 정상 로딩
- 여러 개의 `Parking` 객체 생성 가능
- 주요 필드 값 정상 저장
- 누락 데이터로 프로그램이 비정상 종료되지 않음
- 간단한 테스트 데이터로 결과 확인

---

# 6. 검색 담당 구현 가이드

## 담당 파일

```text
src/search.h
src/search.cpp
```

## 목표

전체 주차장 데이터를 대상으로 자치구 및 주차장명 검색 기능을 구현합니다.

가능하면 검색 성능 향상을 위해 Hash Table을 사용합니다.

## 기능 1. 자치구 검색

입력:

```text
광진구
```

출력:

```text
광진구에 속하는 Parking 목록
```

예상 인터페이스:

```cpp
std::vector<Parking> searchByDistrict(
    const std::string& district
);
```

## 기능 2. 주차장명 검색

예상 인터페이스:

```cpp
std::vector<Parking> searchByName(
    const std::string& name
);
```

## 검색용 자료구조 예

```text
Key       Value
----------------------------
강남구  → 주차장 index 목록
광진구  → 주차장 index 목록
마포구  → 주차장 index 목록
```

가능하면 실제 `Parking` 객체를 중복 저장하기보다는 원본 Vector의 index를 저장하는 방식을 우선 고려합니다.

예:

```text
전체 데이터

parkingLots
0 → A주차장
1 → B주차장
2 → C주차장
3 → D주차장
```

Hash Table:

```text
광진구 → [0, 3]
강남구 → [1, 2]
```

## 완료 기준

- 자치구 검색 가능
- 주차장명 검색 가능
- 존재하지 않는 검색어 처리 가능
- 동일 자치구에 여러 주차장이 있어도 모두 반환
- 시간복잡도 설명 가능

---

# 7. 정렬 / 추천 담당 구현 가이드

## 담당 파일

```text
src/sort.h
src/sort.cpp

src/recommendation.h
src/recommendation.cpp
```

## 기능 1. 요금 기준 정렬

예상 인터페이스:

```cpp
void sortByFee(std::vector<Parking>& parkingLots);
```

기본 요금이 낮은 순서부터 정렬합니다.

```text
1000원
2000원
3000원
5000원
```

## 기능 2. 주차면수 기준 정렬

예상 인터페이스:

```cpp
void sortByCapacity(std::vector<Parking>& parkingLots);
```

주차면수가 많은 순 또는 적은 순 중 하나로 규칙을 통일합니다.

## 기능 3. Top-N 추천

예:

```text
광진구에서 요금이 가장 저렴한 주차장 5개
```

예상 인터페이스:

```cpp
std::vector<Parking> recommendTopNByFee(
    const std::vector<Parking>& parkingLots,
    int n
);
```

Heap / Priority Queue 등의 자료구조 활용을 고려합니다.

## 완료 기준

- 요금 기준 정렬 정상 동작
- 주차면수 기준 정렬 정상 동작
- 동일한 값이 여러 개 존재하는 경우 처리
- 데이터가 없는 경우 처리
- N이 데이터 개수보다 큰 경우 처리
- 사용한 알고리즘의 시간복잡도 설명 가능

---

# 8. 발표 / UI / 테스트 담당 구현 가이드

## 담당 파일

```text
src/ui.h
src/ui.cpp

tests/
README.md
docs/
```

## 목표

사용자가 프로그램 기능을 선택하고 결과를 확인할 수 있도록 메뉴 및 출력 기능을 구현합니다.

예상 메뉴:

```text
==============================
 서울시 공영주차장 안내 프로그램
==============================

1. 전체 주차장 조회
2. 자치구별 검색
3. 주차장명 검색
4. 요금순 정렬
5. 주차면수순 정렬
6. 저렴한 주차장 추천
0. 종료

메뉴 선택:
```

## 출력 예시

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

## 예외처리

다음 상황을 반드시 고려합니다.

```text
잘못된 메뉴 번호
존재하지 않는 자치구
검색 결과 없음
잘못된 숫자 입력
빈 문자열 입력
```

## 테스트

각 기능별 정상 상황과 예외 상황을 확인합니다.

예:

```text
[검색 테스트]

광진구 검색
→ 정상 결과 출력

존재하지 않는 자치구 검색
→ "검색 결과가 없습니다."

빈 문자열
→ 입력 오류 처리
```

## 발표 관련

- 프로그램 구조 정리
- 각 자료구조 사용 이유 정리
- 시간복잡도 정리
- GitHub 개발 과정 정리
- 실행 화면 준비
- 발표자료 작성
- 최종 발표

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
2. 함수 인터페이스 협의
3. 담당 모듈 간 연결
4. `main.cpp` 작성
5. Pull Request 확인
6. Conflict 관리
7. 전체 프로그램 통합
8. 최종 빌드 및 테스트

`main.cpp`에서는 가급적 세부 알고리즘을 직접 구현하지 않고 각 담당 모듈의 함수를 호출합니다.

예:

```cpp
int main()
{
    std::vector<Parking> parkingLots =
        loadParkingData("data/parking.csv");

    // 검색 자료구조 초기화

    while (true)
    {
        int menu = showMenu();

        // 메뉴에 따라
        // search / sort / recommendation 함수 호출
    }

    return 0;
}
```

---

# 10. 모듈 간 구현 원칙

각 기능은 최대한 독립적으로 구현합니다.

잘못된 형태:

```cpp
// search.cpp에서
// 파일을 직접 읽고
// 메뉴까지 출력하고
// 정렬까지 수행
```

권장 형태:

```text
data_loader
→ 데이터만 읽음

search
→ 검색만 수행

sort
→ 정렬만 수행

recommendation
→ 추천만 수행

ui
→ 입력 / 출력만 수행

main
→ 각 기능 연결
```

즉 하나의 모듈이 다른 담당자의 기능까지 모두 처리하지 않습니다.

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
Issue 작성
    ↓
관련 담당자 공유
    ↓
조장 확인
    ↓
변경
```

순으로 진행합니다.

---

# 12. 자료구조 구현 관련 주의사항

본 프로젝트는 자료구조 과목 프로젝트이므로 단순히 프로그램이 동작하는 것뿐만 아니라

```text
왜 이 자료구조를 선택했는가?
어떤 연산을 수행하는가?
시간복잡도는 얼마인가?
다른 자료구조와 비교했을 때 장점은 무엇인가?
```

를 설명할 수 있어야 합니다.

따라서 STL 사용 여부와 직접 구현 범위는 팀 내에서 먼저 통일합니다.

예:

```cpp
std::unordered_map
std::priority_queue
std::sort
```

만 호출하는 방식으로 끝낼지,

Hash Table / Heap / Sorting을 직접 구현할지는 교수님의 평가 기준을 확인한 뒤 결정합니다.

자료구조 자체의 구현 과정이 평가 대상이라면 직접 구현하는 방향을 우선 고려합니다.

---

# 13. GitHub 작업 방법

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
feature/7-fee-sort
feature/10-ui
```

`main`에서 직접 개발하지 않습니다.

---

# 14. Pull Request 전 확인 사항

PR을 생성하기 전에 다음 내용을 확인합니다.

- [ ] 담당 Issue의 기능을 모두 구현했는가?
- [ ] 정상적으로 컴파일되는가?
- [ ] 기본 기능을 직접 테스트했는가?
- [ ] 예외 상황을 확인했는가?
- [ ] 불필요한 파일을 Commit하지 않았는가?
- [ ] 담당하지 않은 공통 코드를 임의로 변경하지 않았는가?
- [ ] 함수 및 주요 로직에 필요한 주석이 있는가?
- [ ] 시간복잡도를 설명할 수 있는가?

---

# 15. 구현 완료 기준

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
PR 생성
+
Code Review
```

---

# 16. 공통 목표

각자의 기능을 따로 구현하는 것이 최종 목표가 아니라,

```text
Data
 ↓
Search
 ↓
Sort / Recommendation
 ↓
UI
```

가 하나의 프로그램으로 정상적으로 연결되는 것이 최종 목표입니다.

기능 또는 공통 구조를 변경해야 하는 경우 혼자 결정하지 않고 관련 담당자와 먼저 공유합니다.
