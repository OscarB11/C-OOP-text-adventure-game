#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <unordered_map>

class Object;
class Enemy;

class Room {
public:
    std::string id;
    std::string desc;
    std::unordered_map<std::string, std::string> exits; // Map of exits from the room
    std::vector<Object> objects;
    std::vector<Enemy> enemies;
};

#endif 
