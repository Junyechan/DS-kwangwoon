# DS-kwangwoon

광운대학교 자료구조 팀 프로젝트

## 프로젝트 주제
서울시 공영주차장 안내 프로그램

서울 데이터 허브에서 제공하는
'서울시 공영주차장 안내 정보' 데이터를 활용하여
사용자가 원하는 조건에 따라 공영주차장을 검색하고 조회할 수 있는 프로그램을 구현한다.

## Data Source
- 서울 데이터 허브 (Seoul Data Hub)
- 서울시 공영주차장 안내 정보

## 개발 환경
- Language: C++
- Version Control: Git / GitHub

## 주요 기능
- 공영주차장 목록 조회
- 자치구별 주차장 검색
- 주차장명 검색
- 요금 기준 검색
- 운영시간 조회
- 조건별 정렬

- ## 팀 역할

| 역할 | 담당 내용 |
|---|---|
| 조장 / 통합 | 전체 구조 설계, 공통 데이터 구조 설계, 모듈 통합, GitHub 관리 |
| 데이터 | 서울 데이터 허브 데이터 분석, 데이터 로딩 및 전처리 |
| 검색 | 자치구 및 주차장명 검색 기능, 검색 자료구조 구현 |
| 정렬 / 추천 | 요금 및 주차면수 정렬, 추천 기능 및 관련 자료구조 구현 |
| 발표 / UI / 테스트 | 사용자 메뉴 및 출력, 테스트, README 및 발표자료 정리 |

## Git Workflow

모든 개발 작업은 Issue 단위로 진행한다.

1. Issue 생성 또는 담당 Issue 확인
2. 최신 `main` 브랜치에서 작업 브랜치 생성
3. 기능 구현 및 Commit
4. GitHub에 Push
5. Pull Request 생성
6. 팀원 Review
7. Squash and Merge
8. Merge 완료 후 작업 브랜치 삭제

### Branch Naming

- 기능 개발: `feature/<issue-number>-<feature-name>`
- 버그 수정: `fix/<issue-number>-<description>`
- 문서 작업: `docs/<issue-number>-<description>`
- 테스트: `test/<issue-number>-<description>`

예시:

```text
feature/3-data-loader
feature/5-district-search
feature/7-fee-sort
fix/13-search-error
docs/12-readme



