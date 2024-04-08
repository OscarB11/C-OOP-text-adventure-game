#include "Room.h"
#include <iostream>

void printRoomDescription(const Room& room) {
    std::cout << "\n-------------------\n";
    std::cout << "You are in " << room.desc << "\n";
    std::cout << "Exits: ";
    for (const auto& exit : room.exits) {
        std::cout << exit.first << " ";
    }
    std::cout << "\n-------------------\n";
}
