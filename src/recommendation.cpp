#include "recommendation.h"

// Heap에 넣는 원소. order는 후보 리스트에서의 위치(0부터)다.
// 요금이 같으면 order가 작은(리스트에서 먼저 나온) 주차장을 앞에 둬서
// sortByFee(stable merge sort) 결과의 앞 N개와 항상 같은 순서가 되게 한다.
struct HeapEntry
{
    ParkingNode* node;
    int order;
};

// 배열 기반 Max Heap
// 크기를 topN으로 유지하면 루트에는 "지금까지 고른 N개 중 가장 뒤로 밀릴 주차장"
// (가장 비싸고, 요금이 같으면 가장 나중에 나온 것)이 온다.
struct ParkingHeap
{
    HeapEntry* data;
    int size;
};

// 마지막 추천에서 비교 함수를 부른 횟수 (복잡도 측정용)
static long long comparisonCount = 0;

// x가 y보다 추천 순위가 높으면(앞에 와야 하면) true
static bool ranksBefore(const HeapEntry& x, const HeapEntry& y)
{
    comparisonCount++;

    int feeX = x.node->data.basicFee;
    int feeY = y.node->data.basicFee;

    if (feeX != feeY)
        return feeX < feeY;

    return x.order < y.order;
}

static void swapEntry(HeapEntry* a, HeapEntry* b)
{
    HeapEntry temp = *a;
    *a = *b;
    *b = temp;
}

static void siftUp(ParkingHeap* heap, int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;

        // 부모가 이미 자식보다 뒤 순위(더 비쌈)면 Max Heap 조건 만족
        if (!ranksBefore(heap->data[parent], heap->data[index]))
            break;

        swapEntry(&heap->data[parent], &heap->data[index]);
        index = parent;
    }
}

static void siftDown(ParkingHeap* heap, int index)
{
    while (true)
    {
        int largest = index;
        int left = index * 2 + 1;
        int right = index * 2 + 2;

        if (left < heap->size
            && ranksBefore(heap->data[largest], heap->data[left]))
            largest = left;

        if (right < heap->size
            && ranksBefore(heap->data[largest], heap->data[right]))
            largest = right;

        if (largest == index)
            break;

        swapEntry(&heap->data[index], &heap->data[largest]);
        index = largest;
    }
}

static void heapPush(ParkingHeap* heap, HeapEntry entry)
{
    heap->data[heap->size] = entry;
    heap->size++;

    siftUp(heap, heap->size - 1);
}

static HeapEntry heapPop(ParkingHeap* heap)
{
    HeapEntry top = heap->data[0];

    heap->size--;
    heap->data[0] = heap->data[heap->size];

    siftDown(heap, 0);

    return top;
}

bool recommendTopNByFee(
    const ParkingList* candidates,
    ParkingList* result,
    int topN
)
{
    comparisonCount = 0;

    if (candidates == nullptr || result == nullptr)
        return false;

    if (topN <= 0 || candidates->head == nullptr)
        return false;

    // 후보보다 많이 요청하면 후보 개수만큼만 추천
    if (topN > candidates->size)
        topN = candidates->size;

    ParkingHeap heap;

    heap.data = new HeapEntry[topN];
    heap.size = 0;

    // 후보를 한 번 순회: O(m log k)
    int order = 0;

    for (ParkingNode* current = candidates->head;
         current != nullptr;
         current = current->next, order++)
    {
        // 요금 정보가 없는 주차장은 "싸다"고 추천할 수 없으므로 제외
        if (current->data.basicFee == FEE_UNKNOWN)
            continue;

        HeapEntry entry = { current, order };

        if (heap.size < topN)
        {
            heapPush(&heap, entry);
        }
        else if (ranksBefore(entry, heap.data[0]))
        {
            // 현재 N개 중 가장 뒤 순위보다 앞이면 교체
            heap.data[0] = entry;
            siftDown(&heap, 0);
        }
    }

    // Heap Sort 마무리: 꺼낸 루트(가장 뒤 순위)를 비워진 맨 뒷칸에 넣으면
    // 배열이 앞에서부터 추천 순서대로 정렬된다.
    int count = heap.size;

    while (heap.size > 0)
    {
        HeapEntry top = heapPop(&heap);
        heap.data[heap.size] = top;
    }

    for (int i = 0; i < count; i++)
        addParking(result, heap.data[i].node->data);

    delete[] heap.data;

    return true;
}

long long getLastRecommendComparisons()
{
    return comparisonCount;
}
