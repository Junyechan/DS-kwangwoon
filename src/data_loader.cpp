#include "data_loader.h"

#include <fstream>

// 컬럼 위치를 저장하는 구조체 (-1이면 해당 컬럼 없음)
struct ColumnIndex
{
    int name;
    int address;
    int district;
    int capacity;
    int basicFee;
    int openTime;
    int closeTime;
    int code;
    int basicTime;
    int addFee;
    int addTime;
    int payType;
};

static const int MAX_FIELDS = 64;

// CSV 한 줄을 필드 배열로 분리한다.
// 큰따옴표로 감싼 필드 안의 쉼표와 "" (따옴표 escape)를 처리한다.
static int splitCsvLine(
    const std::string& line,
    std::string fields[],
    int maxFields
)
{
    int count = 0;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); i++)
    {
        char c = line[i];

        if (inQuotes)
        {
            if (c == '"')
            {
                if (i + 1 < line.size() && line[i + 1] == '"')
                {
                    current += '"';
                    i++;
                }
                else
                {
                    inQuotes = false;
                }
            }
            else
            {
                current += c;
            }
        }
        else if (c == '"')
        {
            inQuotes = true;
        }
        else if (c == ',')
        {
            if (count < maxFields)
                fields[count++] = current;

            current.clear();
        }
        else if (c != '\r')
        {
            current += c;
        }
    }

    if (count < maxFields)
        fields[count++] = current;

    return count;
}

static std::string trim(const std::string& s)
{
    size_t start = 0;
    size_t end = s.size();

    while (start < end && (s[start] == ' ' || s[start] == '\t'))
        start++;

    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t'))
        end--;

    return s.substr(start, end - start);
}

static bool contains(const std::string& s, const char* key)
{
    return s.find(key) != std::string::npos;
}

// 각 필드로 인정하는 컬럼명 (서울 데이터 허브 원본명, 단순화한 이름)
// 이름이 정확히 일치해야 하므로 "버스 기본 주차 요금" 같은 컬럼과 헷갈리지 않는다.
static const char* const NAME_COLUMNS[] = { "주차장명", nullptr };
static const char* const CODE_COLUMNS[] = { "주차장코드", nullptr };
static const char* const ADDRESS_COLUMNS[] = { "주소", nullptr };
static const char* const DISTRICT_COLUMNS[] = { "자치구", nullptr };
static const char* const CAPACITY_COLUMNS[] = { "총 주차면", "주차면수", nullptr };
static const char* const FEE_COLUMNS[] = { "기본 주차 요금", "기본 요금", nullptr };
static const char* const TIME_COLUMNS[] = { "기본 주차 시간(분 단위)", "기본 주차 시간", nullptr };
static const char* const ADD_FEE_COLUMNS[] = { "추가 단위 요금", nullptr };
static const char* const ADD_TIME_COLUMNS[] = { "추가 단위 시간(분 단위)", "추가 단위 시간", nullptr };
static const char* const PAY_TYPE_COLUMNS[] = { "유무료구분명", nullptr };
static const char* const OPEN_COLUMNS[] = { "평일 운영 시작시각(HHMM)", "운영 시작 시간", nullptr };
static const char* const CLOSE_COLUMNS[] = { "평일 운영 종료시각(HHMM)", "운영 종료 시간", nullptr };

// aliases 중 하나와 정확히 같은 헤더의 위치, 없으면 -1
static int findColumn(
    const std::string header[],
    int count,
    const char* const aliases[]
)
{
    for (int i = 0; i < count; i++)
    {
        for (int a = 0; aliases[a] != nullptr; a++)
        {
            if (trim(header[i]) == aliases[a])
                return i;
        }
    }

    return -1;
}

static ColumnIndex findColumns(const std::string header[], int count)
{
    ColumnIndex idx;

    idx.name = findColumn(header, count, NAME_COLUMNS);
    idx.address = findColumn(header, count, ADDRESS_COLUMNS);
    idx.district = findColumn(header, count, DISTRICT_COLUMNS);
    idx.capacity = findColumn(header, count, CAPACITY_COLUMNS);
    idx.basicFee = findColumn(header, count, FEE_COLUMNS);
    idx.openTime = findColumn(header, count, OPEN_COLUMNS);
    idx.closeTime = findColumn(header, count, CLOSE_COLUMNS);
    idx.code = findColumn(header, count, CODE_COLUMNS);
    idx.basicTime = findColumn(header, count, TIME_COLUMNS);
    idx.addFee = findColumn(header, count, ADD_FEE_COLUMNS);
    idx.addTime = findColumn(header, count, ADD_TIME_COLUMNS);
    idx.payType = findColumn(header, count, PAY_TYPE_COLUMNS);

    return idx;
}

// 주소에서 "OO구"로 끝나는 첫 단어를 자치구로 추출한다.
// 예: "광진구 능동로 120" → "광진구"
static std::string extractDistrict(const std::string& address)
{
    std::string token;

    for (size_t i = 0; i <= address.size(); i++)
    {
        if (i == address.size() || address[i] == ' ')
        {
            // "구"는 UTF-8로 3바이트
            if (token.size() >= 6
                && token.compare(token.size() - 3, 3, "구") == 0)
                return token;

            token.clear();
        }
        else
        {
            token += address[i];
        }
    }

    return "";
}

// 숫자가 아닌 문자는 무시하고 정수로 변환한다. 비어 있으면 0.
static int toInt(const std::string& s)
{
    int value = 0;

    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] >= '0' && s[i] <= '9')
            value = value * 10 + (s[i] - '0');
        else if (s[i] == '.')
            break;
    }

    return value;
}

// "900" 또는 "0900" → "09:00"
static std::string formatTime(const std::string& s)
{
    std::string t = trim(s);

    if (t.empty() || contains(t, ":"))
        return t;

    while (t.size() < 4)
        t = "0" + t;

    return t.substr(0, 2) + ":" + t.substr(2, 2);
}

// 1시간 주차했을 때 실제로 내는 요금.
// 주차장마다 기본 시간(5분, 10분, 30분 ...)이 달라 기본 요금끼리는 비교할 수 없다.
// 예: 기본 5분 280원 + 추가 5분당 280원 → 280 + 11 × 280 = 3360원
static int hourlyFee(int basicFee, int basicTime, int addFee, int addTime)
{
    // 기본 시간이 1시간 이상이면 기본 요금이 곧 1시간 요금
    if (basicTime >= 60)
        return basicFee;

    // 기본 시간이 0이면 처음부터 추가 단위 요금만 붙는다.
    // 예: 쌍문2동 5공영주차장 = 기본 0분 0원 + 5분당 100원 → 12 × 100 = 1200원
    if (basicTime <= 0)
    {
        if (addTime <= 0)
            return basicFee;

        return basicFee + (60 + addTime - 1) / addTime * addFee;
    }

    // 추가 단위 정보가 없으면 기본 단위가 반복된다고 본다.
    if (addTime <= 0)
    {
        addFee = basicFee;
        addTime = basicTime;
    }

    int remaining = 60 - basicTime;
    int units = (remaining + addTime - 1) / addTime;  // 올림

    return basicFee + units * addFee;
}

// 한글 없이 ASCII로만 된 문자열인지 (원본 CSV 두 번째 줄의 영문 컬럼명 판별용)
static bool isAscii(const std::string& s)
{
    for (size_t i = 0; i < s.size(); i++)
    {
        if ((unsigned char)s[i] >= 0x80)
            return false;
    }

    return true;
}

static bool isBlank(const std::string& line)
{
    return line.find_first_not_of(" \t\r") == std::string::npos;
}

// 원본 데이터는 노상 주차장을 주차면 1개당 한 행으로 기록하므로
// 같은 주차장코드가 여러 행에 나온다. 코드 → 노드를 기억하는
// 로딩 전용 Hash Table로 중복 행을 찾아 주차면수를 합친다.
static const int CODE_TABLE_SIZE = 1021;

struct CodeEntry
{
    std::string code;
    ParkingNode* node;
    CodeEntry* next;
};

static unsigned int hashCode(const std::string& key)
{
    unsigned int hash = 5381;

    for (size_t i = 0; i < key.size(); i++)
        hash = hash * 33 + (unsigned char)key[i];

    return hash % CODE_TABLE_SIZE;
}

static ParkingNode* findCode(
    CodeEntry* table[],
    const std::string& code
)
{
    CodeEntry* current = table[hashCode(code)];

    while (current != nullptr)
    {
        if (current->code == code)
            return current->node;

        current = current->next;
    }

    return nullptr;
}

static void insertCode(
    CodeEntry* table[],
    const std::string& code,
    ParkingNode* node
)
{
    unsigned int index = hashCode(code);

    CodeEntry* entry = new CodeEntry;

    entry->code = code;
    entry->node = node;
    entry->next = table[index];

    table[index] = entry;
}

static void clearCodeTable(CodeEntry* table[])
{
    for (int i = 0; i < CODE_TABLE_SIZE; i++)
    {
        CodeEntry* current = table[i];

        while (current != nullptr)
        {
            CodeEntry* next = current->next;

            delete current;

            current = next;
        }

        table[i] = nullptr;
    }
}

static std::string getField(
    const std::string fields[],
    int count,
    int index
)
{
    if (index < 0 || index >= count)
        return "";

    return trim(fields[index]);
}

bool loadParkingData(
    const std::string& filename,
    ParkingList* list
)
{
    if (list == nullptr)
        return false;

    std::ifstream file(filename.c_str());

    if (!file.is_open())
        return false;

    std::string line;

    if (!std::getline(file, line))
        return false;

    // UTF-8 BOM 제거
    if (line.size() >= 3
        && (unsigned char)line[0] == 0xEF
        && (unsigned char)line[1] == 0xBB
        && (unsigned char)line[2] == 0xBF)
        line = line.substr(3);

    std::string fields[MAX_FIELDS];
    int count = splitCsvLine(line, fields, MAX_FIELDS);

    ColumnIndex idx = findColumns(fields, count);

    // 주차장명은 필수 컬럼
    if (idx.name < 0)
        return false;

    // 원본 CSV는 두 번째 줄에 영문 컬럼명(seq, prknglot_nm, ...)이 한 번 더 있다.
    // 주차장명 칸이 영문뿐이면 컬럼명 줄로 보고 건너뛰고, 아니면 다시 읽도록 되돌린다.
    std::streampos dataStart = file.tellg();

    if (std::getline(file, line))
    {
        count = splitCsvLine(line, fields, MAX_FIELDS);

        if (!isAscii(getField(fields, count, idx.name)))
            file.seekg(dataStart);
    }

    CodeEntry* codeTable[CODE_TABLE_SIZE] = {};

    while (std::getline(file, line))
    {
        if (isBlank(line))
            continue;

        count = splitCsvLine(line, fields, MAX_FIELDS);

        int capacity = toInt(getField(fields, count, idx.capacity));

        // 노상 주차장은 주차면 1개당 1행이므로, 이미 나온 주차장코드면
        // 나머지 필드는 읽지 않고 주차면수만 더한다.
        std::string code = getField(fields, count, idx.code);
        ParkingNode* existing = findCode(codeTable, code);

        if (existing != nullptr)
        {
            existing->data.capacity += capacity;
            continue;
        }

        Parking p;

        p.name = getField(fields, count, idx.name);

        // 주차장명이 없는 행은 건너뛴다.
        if (p.name.empty())
            continue;

        p.address = getField(fields, count, idx.address);
        p.district = getField(fields, count, idx.district);

        if (p.district.empty())
            p.district = extractDistrict(p.address);

        p.capacity = capacity;
        p.basicFee = hourlyFee(
            toInt(getField(fields, count, idx.basicFee)),
            toInt(getField(fields, count, idx.basicTime)),
            toInt(getField(fields, count, idx.addFee)),
            toInt(getField(fields, count, idx.addTime)));

        // "유료"인데 요금이 0원이면 무료가 아니라 요금 정보가 빠진 것.
        // 0원으로 두면 요금순 정렬 / Top-N 추천에서 1등이 되어 버린다.
        if (p.basicFee == 0
            && getField(fields, count, idx.payType) == "유료")
            p.basicFee = FEE_UNKNOWN;
        p.openTime = formatTime(getField(fields, count, idx.openTime));
        p.closeTime = formatTime(getField(fields, count, idx.closeTime));

        addParking(list, p);

        if (!code.empty())
            insertCode(codeTable, code, list->tail);
    }

    clearCodeTable(codeTable);

    return list->size > 0;
}
