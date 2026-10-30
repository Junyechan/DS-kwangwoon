# DS-kwangwoon

광운대학교 자료구조 팀 프로젝트

---

## 1. 프로젝트 개요

### 프로젝트명

**주차하고 싶어요!**

### 프로젝트 설명

서울 데이터 허브에서 제공하는 **서울시 공영주차장 안내 정보**를 활용하여 사용자가 원하는 조건에 따라 서울시 공영주차장을 검색하고 조회할 수 있는 프로그램을 구현한다.

공영주차장 데이터를 Linked List에 저장하고, Hash Table을 이용하여 자치구 또는 주차장명을 검색한다.

검색된 결과는 요금 및 주차면수 등의 조건으로 정렬할 수 있으며, Heap을 이용하여 요금 기준 Top-N 주차장을 추천한다.

본 프로젝트의 목적은 실제 공공데이터 처리 과정에 Linked List, Hash Table, Merge Sort, Heap 등의 자료구조 및 알고리즘을 적용하는 것이다.

---

## 2. 사용 데이터

### Data Source

- 서울 데이터 허브 (Seoul Data Hub)
- 서울시 공영주차장 안내 정보

### 활용 예정 데이터

공영주차장 데이터 중 프로젝트 기능 구현에 필요한 항목을 선별하여 사용한다.

예정 항목:

- 주차장명
- 주소
- 자치구
- 주차면수
- 기본 요금
- 운영 시작 시간
- 운영 종료 시간

※ 실제 사용하는 데이터 필드는 데이터 분석 후 최종 확정한다.

---

## 3. 주요 기능

- 전체 공영주차장 목록 조회
- 자치구별 주차장 검색
- 주차장명 검색
- 요금 정보 조회
- 운영시간 조회
- 요금 기준 정렬
- 주차면수 기준 정렬
- 요금 기준 Top-N 주차장 추천

---

# 4. 전체 데이터 처리 흐름

```text id="14z3fw"
서울시 공영주차장 데이터
          ↓
      Data Loader
          ↓
    Parking 객체 생성
          ↓
      ParkingList
     (Linked List)
          ↓
   Hash Table 구축
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

# 5. 자료구조 활용 계획

## 5.1 Linked List

전체 공영주차장 데이터를 저장하는 기본 자료구조로 사용한다.

또한 검색 결과를 별도의 Linked List로 저장하여 정렬 및 추천 기능에 활용한다.

### 구조

```text id="te8dvc"
ParkingList

head
 ↓
[Parking]
    ↓
[Parking]
    ↓
[Parking]
    ↓
  ...
    ↓
nullptr
 ↑
tail
```

### 주요 연산

- 주차장 데이터 삽입
- 전체 데이터 순회
- 노드 탐색
- 리스트 초기화
- 리스트 메모리 해제

### 사용 이유

공영주차장 데이터의 개수가 가변적이기 때문에 동적으로 데이터를 저장할 수 있는 Linked List를 사용한다.

또한 노드 간 연결을 변경하여 데이터를 정렬할 수 있으므로 Merge Sort와 함께 활용하기 적합하다.

---

## 5.2 Hash Table

자치구 또는 주차장명을 기준으로 주차장을 빠르게 검색하기 위해 사용한다.

### 주요 연산

- Key 삽입
- Key 검색
- 검색 결과 조회
- 충돌 처리

### 활용 예

```text id="5gqhhe"
광진구
 ├─ 주차장 A
 ├─ 주차장 B
 └─ 주차장 C

강남구
 ├─ 주차장 D
 └─ 주차장 E
```

Hash Table에는 동일한 `Parking` 객체를 다시 저장하기보다 원본 Linked List의 `ParkingNode`를 참조하는 방식을 고려한다.

### 사용 이유

전체 Linked List를 매 검색마다 처음부터 탐색할 경우 검색에 O(n)의 시간이 필요하다.

Hash Table을 구축하면 자치구 및 주차장명과 같은 Key를 이용하여 평균적으로 빠른 검색이 가능하다.

### 예상 시간복잡도

평균 검색:

```text id="lr655f"
O(1)
```

단, Hash 충돌 상황에 따라 실제 시간복잡도는 달라질 수 있다.

---

## 5.3 Merge Sort

검색 결과를 요금 또는 주차면수를 기준으로 정렬하기 위해 사용한다.

### 정렬 기능

- 기본 요금 오름차순
- 주차면수 내림차순

### 예시

요금 기준:

```text id="9m8t6j"
1000원
  ↓
2000원
  ↓
3000원
  ↓
5000원
```

주차면수 기준:

```text id="3eo7x7"
300면
 ↓
200면
 ↓
100면
 ↓
50면
```

### 사용 이유

Linked List는 배열과 달리 임의 위치 접근이 어렵지만 노드의 연결을 변경하기 쉽다.

Merge Sort는 리스트를 분할한 뒤 정렬된 리스트를 다시 연결하는 방식이므로 Linked List 정렬에 적합하다.

### 예상 시간복잡도

```text id="yva32x"
O(n log n)
```

---

## 5.4 Heap

검색된 주차장 중 요금이 가장 저렴한 Top-N 주차장을 선택하기 위해 사용한다.

### 예시

```text id="2u47qf"
광진구 검색 결과: 50개

사용자 요청:
가장 저렴한 주차장 5개

Top-N = 5
```

Heap의 크기를 최대 N으로 유지하면서 후보 데이터를 처리하여 저렴한 주차장 N개를 선택한다.

### 주요 연산

- Heap 삽입
- Heap 삭제
- Heapify
- Top-N 후보 관리

### 사용 이유

전체 데이터를 모두 정렬한 뒤 앞의 N개를 선택하는 방식보다, 필요한 N개의 후보만 Heap으로 관리하여 Top-N 추천 기능을 구현하기 위해 사용한다.

### 예상 시간복잡도

추천 후보가 `m`개이고 추천 개수가 `k`개일 경우:

```text id="hmokdf"
O(m log k)
```

---

# 6. 공통 데이터 구조

각 공영주차장은 `Parking` 구조체로 표현한다.

```cpp id="hyu6ju"
struct Parking
{
    std::string name;
    std::string address;
    std::string district;

    int capacity;
    int basicFee;   // 기본 주차 요금 (원본 그대로)
    int hourlyFee;  // 1시간 주차 시 실제 요금, 모르면 FEE_UNKNOWN(-1)

    std::string openTime;
    std::string closeTime;
};
```

※ 실제 필드는 서울시 공영주차장 데이터 분석 후 최종 확정한다.

---

## 6.1 Parking Linked List

공영주차장 데이터를 저장하기 위해 다음 구조를 사용한다.

```cpp id="pz8gsd"
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

### 기본 연산

```cpp id="ljhn2r"
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
```

---

# 7. 주요 모듈

## Data Loader

서울시 공영주차장 데이터 파일을 읽어 `Parking` 객체로 변환하고 `ParkingList`에 저장한다.

```text id="e1u09f"
데이터 파일
    ↓
Parking 생성
    ↓
addParking()
    ↓
ParkingList
```

담당 파일:

```text id="wx0pxp"
data_loader.h
data_loader.cpp
```

---

## Search

Hash Table을 구축하여 자치구 및 주차장명 검색 기능을 제공한다.

담당 파일:

```text id="a7uubh"
search.h
search.cpp
```

주요 기능:

```text id="t09jc7"
Hash Table 구축
자치구 검색
주차장명 검색
```

---

## Sort

검색 결과 `ParkingList`를 정렬한다.

담당 파일:

```text id="0pvz6v"
sort.h
sort.cpp
```

주요 기능:

```text id="fuxuxa"
요금 기준 Merge Sort
주차면수 기준 Merge Sort
```

---

## Recommendation

검색 결과를 대상으로 Heap을 이용하여 Top-N 주차장을 추천한다.

담당 파일:

```text id="rtdqpu"
recommendation.h
recommendation.cpp
```

주요 기능:

```text id="sd2xs1"
요금 기준 Top-N 추천
Heap 관리
```

---

## Main

각 모듈을 연결하고 사용자 입력 및 콘솔 출력을 처리한다.

담당 파일:

```text id="k5k3ec"
main.cpp
```

예상 메뉴:

```text id="n32dml"
================================
 서울시 공영주차장 안내 프로그램
================================

1. 전체 주차장 조회
2. 자치구별 검색
3. 주차장명 검색
4. 요금순 정렬
5. 주차면수순 정렬
6. 요금 기준 Top-N 추천
0. 종료
```

별도의 UI 모듈은 구현하지 않으며 콘솔 출력 방식으로 결과를 제공한다.

---

# 8. 프로젝트 구조

```text id="f69u85"
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

---

# 9. 역할 분담

| 역할 | 주요 담당 |
|---|---|
| 조장 / 통합 | 프로젝트 구조, Parking 및 ParkingList, main.cpp, 전체 기능 통합, GitHub 관리 |
| 데이터 | 데이터 컬럼 분석, 데이터 전처리 및 Data Loader 구현 |
| 검색 | Hash Table 구현, 자치구 / 주차장명 검색 |
| 정렬 / 추천 | Merge Sort 및 Heap 기반 Top-N 추천 |
| 발표 / 테스트 | 기능 테스트, README 정리, 발표자료 작성 및 발표 |

---

# 10. 개발 Issue

```text id="wr0xye"
#1 프로젝트 기본 구조 생성
담당: 조장

#2 서울시 공영주차장 데이터 컬럼 분석
담당: 데이터

#3 사용할 데이터 필드 선정 및 Parking 구조체 설계
담당: 조장 + 데이터

#4 ParkingList(Linked List) 기본 구조 및 연산 구현
담당: 조장

#5 공영주차장 데이터 로더 구현
담당: 데이터

#6 Hash Table 검색 인덱스 구축
담당: 검색

#7 자치구별 주차장 검색 기능 구현
담당: 검색

#8 주차장명 검색 기능 구현
담당: 검색

#9 요금 기준 Linked List 정렬 기능 구현
담당: 정렬 / 추천

#10 주차면수 기준 Linked List 정렬 기능 구현
담당: 정렬 / 추천

#11 Heap 기반 Top-N 주차장 추천 기능 구현
담당: 정렬 / 추천

#12 콘솔 메뉴 및 결과 출력 구현
담당: 조장

#13 전체 기능 통합
담당: 조장

#14 기능별 테스트 및 예외처리
담당: 발표 / 테스트 + 각 기능 담당자

#15 통합 테스트
담당: 전체

#16 README 및 발표 자료 작성
담당: 발표 / 테스트 + 전체 검토
```

---

# 11. GitHub Workflow

모든 개발은 Issue 단위로 진행한다.

```text id="tvxhky"
Issue 생성 / 확인
        ↓
최신 main 확인
        ↓
작업 Branch 생성
        ↓
기능 구현
        ↓
테스트
        ↓
Commit
        ↓
Push
        ↓
Pull Request
        ↓
Code Review
        ↓
Squash and Merge
```

`main` 브랜치에는 직접 개발하거나 직접 Push하지 않는다.

---

## Branch Naming

예:

```text id="kcj6iv"
feature/4-parking-list
feature/5-data-loader
feature/6-search-index
feature/7-district-search
feature/9-fee-sort
feature/11-topn-recommendation

test/15-integration-test

docs/16-readme
```

---

## Commit Convention

```text id="mo1spp"
feat: 새로운 기능 구현
fix: 버그 수정
docs: 문서 수정
test: 테스트 코드
refactor: 코드 구조 개선
chore: 기타 설정 및 작업
```

예:

```text id="y1ulel"
feat: implement parking linked list
feat: add district search
fix: handle empty search result
docs: update README
```

---

# 12. Pull Request

기능 구현 후 Pull Request를 생성한다.

PR 생성 전 확인 사항:

- 정상적으로 컴파일되는가?
- 담당 Issue 기능이 구현되었는가?
- 정상 입력을 테스트했는가?
- 예외 입력을 테스트했는가?
- 불필요한 파일이 포함되어 있지 않은가?
- 공통 자료구조를 임의로 수정하지 않았는가?
- 동적 할당한 메모리가 정상적으로 해제되는가?
- 사용한 자료구조와 시간복잡도를 설명할 수 있는가?

PR Review 완료 후 **Squash and Merge**를 사용한다.

---

# 13. 구현 원칙

각 모듈은 자신의 역할만 담당한다.

```text id="7g9a0x"
parking_list
→ Linked List 관리

data_loader
→ 데이터 로딩

search
→ Hash Table 검색

sort
→ Merge Sort

recommendation
→ Heap 기반 추천

main
→ 사용자 입력
→ 모듈 연결
→ 결과 출력
```

예를 들어 `search.cpp`에서 데이터 파일을 직접 읽거나 정렬까지 수행하지 않는다.

공통 데이터 구조 및 함수 인터페이스 변경이 필요한 경우 담당자와 조장에게 먼저 공유한 뒤 수정한다.

---

# 14. 개발 환경

- Language: C++
- Version Control: Git
- Repository: GitHub
- Data: Seoul Data Hub
- Interface: Console

---

# 15. 프로젝트 목표

본 프로젝트는 프로그램 기능 구현뿐만 아니라 실제 데이터 처리 과정에서 자료구조를 적절하게 선택하고 활용하는 것을 목표로 한다.

프로젝트 종료 시 모든 팀원이 다음 내용을 설명할 수 있도록 한다.

- Linked List를 사용한 이유
- Hash Table을 사용한 이유
- Merge Sort를 사용한 이유
- Heap을 사용한 이유
- 각 자료구조의 주요 연산
- 각 기능의 시간복잡도
- 전체 프로그램의 데이터 처리 흐름


---

# 빌드 및 실행

프로젝트 루트(`README.md`가 있는 폴더)에서 실행합니다.

```bash
# 프로그램
g++ -std=c++11 -Isrc src/*.cpp -o parking
./parking                      # 기본: data/parking.csv
./parking data/다른파일.csv     # 다른 데이터 파일 지정

# 테스트
g++ -std=c++11 -Isrc tests/test_all.cpp src/parking_list.cpp src/data_loader.cpp src/search.cpp src/sort.cpp src/recommendation.cpp -o run_tests
./run_tests
```

- `data/parking.csv`는 서울 데이터 허브 원본 파일(UTF-8, 2,181행)이고, `tests/sample_parking.csv`는 테스트용 샘플(26개)입니다.
- 데이터 로더는 헤더 이름(주차장명, 주차장코드, 주소, 총 주차면, 기본 주차 요금/시간, 평일 운영 시작/종료시각)으로 컬럼을 찾습니다. 자치구는 주소에서 "OO구"를 추출합니다.
- 원본 데이터 처리 규칙:
  - 두 번째 줄의 영문 컬럼명(`seq, prknglot_nm, ...`)은 건너뜁니다.
  - 노상 주차장은 주차면 1개당 1행으로 기록되어 있으므로, **같은 주차장코드의 행을 하나로 합치고 주차면수를 더합니다** (2,181행 → 842개).
  - 기본 시간이 주차장마다 달라(5분, 10분, 30분 등) `basicFee`는 원본 기본 요금 그대로 두고, **1시간 주차 시 실제 요금(기본 요금 + 추가 단위 요금)은 `hourlyFee`에 따로 저장**합니다. 정렬과 Top-N 추천은 `hourlyFee`를 기준으로 합니다. 예: 기본 5분 280원, 추가 5분당 280원 → 3,360원.
- 서울 데이터 허브 CSV는 보통 CP949(EUC-KR)로 저장되어 있으므로 **UTF-8로 다시 저장**해서 사용합니다. Windows 콘솔에서 한글이 깨지면 `chcp 65001`을 먼저 실행합니다.
