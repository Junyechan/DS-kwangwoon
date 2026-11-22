# DS-kwangwoon

광운대학교 자료구조 팀 프로젝트

---

# 1. 프로젝트 개요

## 프로젝트명

**주차하고 싶어요!**

서울시 공영주차장 탐색 및 맞춤형 추천 시스템

## 프로젝트 설명

서울 데이터 허브에서 제공하는 **서울시 공영주차장 안내 정보**를 활용하여 사용자가 원하는 조건에 따라 공영주차장을 검색하고, 검색 결과를 정렬하거나 저렴한 주차장을 추천하는 콘솔 프로그램이다.

공영주차장 데이터는 직접 구현한 Linked List에 저장하고, 자치구와 주차장명을 Key로 하는 Hash Table을 구축하여 검색한다.

검색된 결과는 Linked List Merge Sort를 이용하여 1시간 요금 또는 주차면수를 기준으로 정렬하며, Heap을 이용하여 1시간 요금이 저렴한 Top-N 주차장을 추천한다.

본 프로젝트의 목적은 실제 공공데이터 처리 과정에 **Linked List, Hash Table, Merge Sort, Heap** 등의 자료구조와 알고리즘을 적용하고 각 구조의 특성과 시간복잡도를 비교·분석하는 것이다.

---

# 2. 사용 데이터

## Data Source

- 서울 데이터 허브 (Seoul Data Hub)
- 서울시 공영주차장 안내 정보
- 파일: `data/parking.csv`

프로그램에서 사용하는 `data/parking.csv`는 한글 헤더와 영문 컬럼명 행을 제외하면 **2,181개의 데이터 행**으로 구성되며, 각 행은 **40개의 원본 컬럼**을 가진다.

프로젝트에서는 이 가운데 기능 구현에 필요한 11개 원본 컬럼을 직접 사용한다.

| 원본 컬럼 | 용도 |
|---|---|
| 주차장코드 | 동일 주차장 데이터 병합 |
| 주차장명 | 주차장명 검색 |
| 주소 | 주소 출력 및 자치구 추출 |
| 총 주차면 | 주차면수 저장 및 정렬 |
| 기본 주차 요금 | 1시간 요금 계산 |
| 기본 주차 시간 | 1시간 요금 계산 |
| 추가 단위 요금 | 1시간 요금 계산 |
| 추가 단위 시간 | 1시간 요금 계산 |
| 유무료구분명 | 무료/유료 및 요금 정보 판정 |
| 평일 운영 시작시각 | 운영시간 출력 |
| 평일 운영 종료시각 | 운영시간 출력 |

원본 데이터에는 자치구와 1시간 요금 컬럼이 별도로 존재하지 않기 때문에 프로그램에서 다음 값을 추가로 생성한다.

- `district`: 주소에서 자치구 추출
- `hourlyFee`: 기본·추가 요금과 적용 시간을 이용하여 계산한 1시간 요금

## 데이터 전처리

노상 주차장의 경우 동일한 주차장코드가 여러 행으로 기록되어 있다.

프로그램은 같은 주차장코드를 가진 행을 하나의 주차장으로 병합하고 각 행의 주차면수를 합산한다.

```text
원본 데이터 2,181행
        ↓
동일 주차장코드 병합
        ↓
최종 Parking 데이터 842개
```

자세한 데이터 분석 결과는 다음 문서에서 확인할 수 있다.

```text
docs/PARKING_DATA_ANALYSIS.md
```

---

# 3. 주요 기능

- 전체 공영주차장 목록 조회
- 자치구별 공영주차장 검색
- 주차장명 정확 일치 검색
- 1시간 요금 정보 조회
- 평일 운영시간 조회
- 1시간 요금 기준 오름차순 정렬
- 주차면수 기준 내림차순 정렬
- 1시간 요금 기준 Top-N 주차장 추천

---

# 4. 전체 데이터 처리 흐름

```text
서울시 공영주차장 CSV
          ↓
      Data Loader
          ↓
    Parking 객체 생성
          ↓
 동일 주차장코드 병합
          ↓
      ParkingList
     (Linked List)
          ↓
   Hash Table 구축
  ┌───────┴───────┐
  ↓               ↓
자치구 Key      주차장명 Key
  └───────┬───────┘
          ↓
       검색
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

# 5. 자료구조 및 알고리즘

## 5.1 Linked List

전체 공영주차장 데이터와 검색 결과를 저장하기 위한 기본 자료구조이다.

```text
ParkingList

head
 ↓
[Parking] → [Parking] → [Parking] → ... → nullptr
                                        ↑
                                       tail
```

### 주요 연산

- 노드 생성
- 데이터 삽입
- 전체 데이터 순회
- 리스트 초기화
- 리스트 비우기
- 동적 메모리 해제

### 선택 이유

공영주차장 데이터 개수가 가변적이므로 동적으로 데이터를 관리할 수 있다.

또한 배열처럼 원소를 이동할 필요 없이 `next` 연결 관계를 변경할 수 있어 Merge Sort를 적용하기 적합하다.

---

## 5.2 Hash Table

자치구와 주차장명을 빠르게 검색하기 위해 각각 하나의 Hash Table을 사용한다.

구현 방식:

- 문자열 Hash 함수: djb2
- Bucket 수: 211
- 충돌 처리: Separate Chaining
- 인덱스에서는 원본 `ParkingNode*`를 참조

```text
district Hash Table

"광진구"
    ↓
ParkingRefNode → ParkingRefNode → ...

"강남구"
    ↓
ParkingRefNode → ...
```

Hash Table에는 `Parking` 객체 전체를 중복 저장하지 않고 원본 Linked List의 `ParkingNode*`를 참조한다.

검색 결과는 이후 정렬이나 추천 과정에서 원본 데이터가 변경되지 않도록 별도의 `ParkingList`에 복사한다.

### 시간복잡도

검색 결과 수를 `k`라고 할 경우 평균적으로

```text
O(1 + k)
```

이다.

Hash Table 조회 자체는 평균 O(1)이지만 검색된 `k`개의 주차장을 결과 Linked List로 복사하기 위해 O(k)가 추가된다.

모든 Key가 하나의 Bucket에 집중되는 최악의 경우 검색 시간은 O(n)이 될 수 있다.

자세한 분석은 다음 문서에서 확인할 수 있다.

```text
docs/SEARCH_HASH_ANALYSIS.md
```

---

## 5.3 Merge Sort

검색 결과를 다음 기준으로 정렬한다.

- `hourlyFee` 오름차순
- `capacity` 내림차순

요금 정보가 없는 주차장(`FEE_UNKNOWN`)은 요금 정렬 시 뒤쪽으로 이동한다.

### 선택 이유

Linked List에서는 임의 위치 접근이 어렵기 때문에 배열 기반 정렬 알고리즘보다 노드 연결을 변경하는 Merge Sort가 적합하다.

검색 결과가 `m`개라면 시간복잡도는

```text
O(m log m)
```

이다.

---

## 5.4 Heap

검색된 주차장 중 `hourlyFee`가 가장 낮은 Top-N 주차장을 선택한다.

검색 결과가 `m`개이고 사용자가 요청한 추천 개수가 `k`개라면 최대 `k`개의 후보만 Heap에 유지한다.

```text
검색 결과 m개
      ↓
크기 k의 Heap 유지
      ↓
저렴한 Top-k 선택
```

요금 정보가 없는 주차장은 추천 대상에서 제외된다.

### 시간복잡도

```text
O(m log k)
```

전체 `m`개를 모두 정렬하는 O(m log m) 방식 대신 필요한 `k`개의 후보만 관리한다.

Merge Sort 및 Heap에 대한 자세한 분석은 다음 문서에서 확인할 수 있다.

```text
docs/SORT_TOPN_ANALYSIS.md
```

---

# 6. 공통 데이터 구조

각 공영주차장은 `Parking` 구조체로 표현한다.

```cpp
const int FEE_UNKNOWN = -1;

struct Parking
{
    std::string name;
    std::string address;
    std::string district;

    int capacity;

    int basicFee;
    int hourlyFee;

    std::string openTime;
    std::string closeTime;
};
```

- `basicFee`: CSV에 기록된 기본 주차 요금
- `hourlyFee`: 1시간 주차 시 계산되는 요금
- `FEE_UNKNOWN`: 유료 주차장이지만 요금 정보를 계산할 수 없는 경우

Linked List는 다음 구조를 사용한다.

```cpp
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
```

---

# 7. 주요 모듈

## Parking List

담당 파일:

```text
src/parking.h
src/parking_list.h
src/parking_list.cpp
```

역할:

- `Parking` 데이터 정의
- Linked List 생성 및 삽입
- 리스트 초기화
- 리스트 메모리 관리

---

## Data Loader

담당 파일:

```text
src/data_loader.h
src/data_loader.cpp
```

역할:

- CSV 파일 파싱
- Parking 객체 생성
- 동일 주차장코드 병합
- 자치구 추출
- 1시간 요금 계산

---

## Search

담당 파일:

```text
src/search.h
src/search.cpp
```

역할:

- Hash Table 검색 인덱스 구축
- 자치구 정확 일치 검색
- 주차장명 정확 일치 검색
- Hash Table 메모리 관리

---

## Sort

담당 파일:

```text
src/sort.h
src/sort.cpp
```

역할:

- Linked List Merge Sort
- 1시간 요금 오름차순 정렬
- 주차면수 내림차순 정렬
- 비교 횟수 측정

---

## Recommendation

담당 파일:

```text
src/recommendation.h
src/recommendation.cpp
```

역할:

- Heap 기반 Top-N 추천
- 1시간 요금 기준 후보 관리
- 비교 횟수 측정

---

## Main

담당 파일:

```text
src/main.cpp
```

역할:

- 전체 모듈 연결
- 사용자 입력 처리
- 검색 결과 출력
- 정렬 및 추천 메뉴 제공

---

# 8. 콘솔 메뉴

프로그램 실행 후 메인 메뉴가 출력된다.

```text
[메인 메뉴]
1. 전체 주차장 목록
2. 자치구로 검색
3. 주차장명으로 검색
0. 종료
```

자치구 또는 주차장명 검색에 성공하면 검색 결과에 대해 다음 메뉴를 사용할 수 있다.

```text
[검색 결과 메뉴]
1. 1시간 요금 낮은 순 정렬
2. 주차면수 많은 순 정렬
3. 요금 기준 Top-N 추천
0. 메인 메뉴로
```

정렬과 추천 실행 후에는 Merge Sort 또는 Heap에서 발생한 비교 횟수도 함께 출력한다.

---

# 9. 프로젝트 구조

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
│   ├── parking_list.h
│   ├── parking_list.cpp
│   ├── data_loader.h
│   ├── data_loader.cpp
│   ├── search.h
│   ├── search.cpp
│   ├── sort.h
│   ├── sort.cpp
│   ├── recommendation.h
│   └── recommendation.cpp
│
├── data/
│   ├── parking.csv
│   └── README.md
│
├── tests/
│   ├── test_all.cpp
│   ├── test_search.cpp
│   ├── benchmark.cpp
│   ├── benchmark_search.cpp
│   ├── sample_parking.csv
│   └── README.md
│
└── docs/
    ├── IMPLEMENTATION_GUIDE.md
    ├── PARKING_DATA_ANALYSIS.md
    ├── SEARCH_HASH_ANALYSIS.md
    └── SORT_TOPN_ANALYSIS.md
```

---

# 10. 역할 분담

| 역할 | 주요 담당 |
|---|---|
| 조장 / 통합 | 프로젝트 구조 설계, Parking 및 ParkingList, main.cpp, 전체 기능 통합, GitHub 관리 |
| 데이터 | 데이터 컬럼 분석, 전처리 및 Data Loader |
| 검색 | Hash Table, 자치구 및 주차장명 검색 |
| 정렬 / 추천 | Merge Sort 및 Heap 기반 Top-N 추천 |
| 발표 / 테스트 | 기능 테스트, 문서 정리, 발표자료 작성 및 발표 |

---

# 11. 주요 시간복잡도

`n`은 전체 주차장 수, `m`은 검색 결과 수, `k`는 Top-N 추천 개수이다.

| 기능 | 시간복잡도 |
|---|---:|
| 전체 데이터 순회 | O(n) |
| Hash Table 구축 | O(n) |
| Hash 검색 | 평균 O(1 + k), 최악 O(n) |
| Merge Sort | O(m log m) |
| Heap Top-N 추천 | O(m log k) |

---

# 12. 빌드 및 실행

프로젝트는 **C++20**을 기준으로 한다.

프로젝트 루트에서 다음 명령을 실행한다.

## 프로그램 빌드

```bash
g++ -std=c++20 -Isrc src/*.cpp -o parking
```

## 실행

기본 데이터 파일 사용:

```bash
./parking
```

기본적으로 다음 파일을 읽는다.

```text
data/parking.csv
```

다른 CSV 파일을 지정할 수도 있다.

```bash
./parking data/다른파일.csv
```

---

# 13. 테스트

## 전체 기능 테스트

```bash
g++ -std=c++20 -Isrc \
tests/test_all.cpp \
src/parking_list.cpp \
src/data_loader.cpp \
src/search.cpp \
src/sort.cpp \
src/recommendation.cpp \
-o run_tests

./run_tests
```

## Hash Table 검색 테스트

```bash
g++ -std=c++20 -Isrc \
tests/test_search.cpp \
src/parking_list.cpp \
src/search.cpp \
-o run_test_search

./run_test_search
```

정상적으로 모든 테스트가 통과하면 다음과 같이 출력된다.

```text
ALL PASSED
```

---

# 14. 개발 환경

- Language: C++20
- Compiler: GNU g++
- Version Control: Git
- Repository: GitHub
- Data: Seoul Data Hub
- Interface: Console
- Target Environment: Ubuntu / Windows

---

# 15. 프로젝트 핵심

본 프로젝트의 전체 흐름은 다음과 같다.

```text
공공데이터 로딩
      ↓
Linked List 저장
      ↓
Hash Table 검색
      ↓
검색 결과 Linked List
      ↓
Merge Sort / Heap
      ↓
정렬 및 Top-N 추천
```

이를 통해 실제 서울시 공영주차장 데이터에 자료구조를 적용하고,

- Linked List를 이용한 동적 데이터 관리
- Hash Table을 이용한 빠른 정확 일치 검색
- Merge Sort를 이용한 Linked List 정렬
- Heap을 이용한 효율적인 Top-N 선택

을 구현하였다.
