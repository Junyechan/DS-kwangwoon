#include "sort.h"

// 두 Parking을 비교하는 함수 포인터 타입.
// a가 b보다 앞에 와야 하거나 같으면 true를 반환한다.
// (같을 때 true를 반환해야 stable sort가 된다.)
typedef bool (*ParkingCompare)(const Parking& a, const Parking& b);

// 마지막 정렬에서 비교 함수를 부른 횟수 (복잡도 측정용)
static long long comparisonCount = 0;

// 요금 정보 없는 주차장(FEE_UNKNOWN)은 요금과 상관없이 맨 뒤로 보낸다.
static bool feeAscending(const Parking& a, const Parking& b)
{
    if (b.hourlyFee == FEE_UNKNOWN)
        return true;

    if (a.hourlyFee == FEE_UNKNOWN)
        return false;

    return a.hourlyFee <= b.hourlyFee;
}

static bool capacityDescending(const Parking& a, const Parking& b)
{
    return a.capacity >= b.capacity;
}

// slow / fast 포인터로 리스트의 중간을 찾아 두 개로 나눈다.
// head 쪽 리스트는 그대로, 뒤쪽 리스트의 시작 노드를 반환한다.
static ParkingNode* splitHalf(ParkingNode* head)
{
    ParkingNode* slow = head;
    ParkingNode* fast = head->next;

    while (fast != nullptr && fast->next != nullptr)
    {
        slow = slow->next;
        fast = fast->next->next;
    }

    ParkingNode* second = slow->next;
    slow->next = nullptr;

    return second;
}

// 정렬된 두 리스트를 next 연결만 바꿔서 하나로 합친다.
// 데이터 복사 없이 노드 연결만 재구성한다.
static ParkingNode* merge(
    ParkingNode* a,
    ParkingNode* b,
    ParkingCompare compare
)
{
    ParkingNode dummy;
    ParkingNode* tail = &dummy;

    dummy.next = nullptr;

    while (a != nullptr && b != nullptr)
    {
        comparisonCount++;

        if (compare(a->data, b->data))
        {
            tail->next = a;
            a = a->next;
        }
        else
        {
            tail->next = b;
            b = b->next;
        }

        tail = tail->next;
    }

    tail->next = (a != nullptr) ? a : b;

    return dummy.next;
}

static ParkingNode* mergeSort(
    ParkingNode* head,
    ParkingCompare compare
)
{
    // 노드가 0개 또는 1개면 이미 정렬된 상태
    if (head == nullptr || head->next == nullptr)
        return head;

    ParkingNode* second = splitHalf(head);

    ParkingNode* left = mergeSort(head, compare);
    ParkingNode* right = mergeSort(second, compare);

    return merge(left, right, compare);
}

static void sortList(ParkingList* list, ParkingCompare compare)
{
    comparisonCount = 0;

    if (list == nullptr || list->head == nullptr)
        return;

    list->head = mergeSort(list->head, compare);

    // 정렬 후 마지막 노드를 다시 찾아 tail 갱신
    ParkingNode* current = list->head;

    while (current->next != nullptr)
        current = current->next;

    list->tail = current;
}

void sortByFee(ParkingList* list)
{
    sortList(list, feeAscending);
}

void sortByCapacity(ParkingList* list)
{
    sortList(list, capacityDescending);
}

long long getLastSortComparisons()
{
    return comparisonCount;
}
