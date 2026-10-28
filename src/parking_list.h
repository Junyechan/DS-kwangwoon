#ifndef PARKING_LIST_H
#define PARKING_LIST_H

#include "parking.h"

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

void createParkingList(ParkingList* list);

bool addParking(
    ParkingList* list,
    const Parking& parking
);

void clearParkingList(ParkingList* list);

void destroyParkingList(ParkingList* list);

bool isParkingListEmpty(
    const ParkingList* list
);

#endif
