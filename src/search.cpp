#include "search.h"

// Hash Table 버킷 개수 (소수를 사용하면 해시값이 고르게 분산된다)
static const int TABLE_SIZE = 211;

// 버킷 안의 체인 노드 (Separate Chaining)
// Parking을 복사하지 않고 원본 ParkingNode를 참조한다.
struct ParkingRefNode
{
    std::string key;
    ParkingNode* parking;
    ParkingRefNode* next;
};

// tails: 버킷별 마지막 노드. 체인 끝에 O(1)로 추가하기 위해 유지한다.
struct HashTable
{
    ParkingRefNode* buckets[TABLE_SIZE];
    ParkingRefNode* tails[TABLE_SIZE];
};

static HashTable districtTable = {};
static HashTable nameTable = {};

// djb2 문자열 해시 함수
static unsigned int hashString(const std::string& key)
{
    unsigned int hash = 5381;

    for (size_t i = 0; i < key.size(); i++)
        hash = hash * 33 + (unsigned char)key[i];

    return hash % TABLE_SIZE;
}

static void insertKey(
    HashTable* table,
    const std::string& key,
    ParkingNode* parking
)
{
    if (key.empty())
        return;

    unsigned int index = hashString(key);

    ParkingRefNode* node = new ParkingRefNode;

    node->key = key;
    node->parking = parking;
    node->next = nullptr;

    // 체인의 끝에 추가해야 원본 리스트 순서가 유지된다.
    if (table->buckets[index] == nullptr)
        table->buckets[index] = node;
    else
        table->tails[index]->next = node;

    table->tails[index] = node;
}

// result를 비운 뒤 key가 일치하는 주차장을 복사한다.
// 해시가 같은 다른 key(충돌)는 문자열 비교로 걸러낸다.
static bool findKey(
    const HashTable* table,
    const std::string& key,
    ParkingList* result
)
{
    if (result == nullptr)
        return false;

    clearParkingList(result);

    bool found = false;
    ParkingRefNode* current = table->buckets[hashString(key)];

    while (current != nullptr)
    {
        if (current->key == key)
        {
            addParking(result, current->parking->data);
            found = true;
        }

        current = current->next;
    }

    return found;
}

static void clearTable(HashTable* table)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        ParkingRefNode* current = table->buckets[i];

        while (current != nullptr)
        {
            ParkingRefNode* next = current->next;

            delete current;

            current = next;
        }

        table->buckets[i] = nullptr;
        table->tails[i] = nullptr;
    }
}

void buildSearchIndex(const ParkingList* list)
{
    destroySearchIndex();

    if (list == nullptr)
        return;

    ParkingNode* current = list->head;

    while (current != nullptr)
    {
        insertKey(&districtTable, current->data.district, current);
        insertKey(&nameTable, current->data.name, current);

        current = current->next;
    }
}

bool searchByDistrict(
    const std::string& district,
    ParkingList* result
)
{
    return findKey(&districtTable, district, result);
}

bool searchByName(
    const std::string& name,
    ParkingList* result
)
{
    return findKey(&nameTable, name, result);
}

void destroySearchIndex()
{
    clearTable(&districtTable);
    clearTable(&nameTable);
}
