#ifndef PARKING_H
#define PARKING_H

#include <string>

struct Parking
{
    std::string name;
    std::string address;
    std::string district;

    int capacity;
    int basicFee;

    std::string openTime;
    std::string closeTime;
};

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

#endif
