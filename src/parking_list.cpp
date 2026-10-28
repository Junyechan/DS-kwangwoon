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
