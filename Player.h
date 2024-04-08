// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>

class Player {
public:
    std::string currentRoom;
    std::string initialRoom;
    std::vector<std::string> inventory;
    std::vector<std::string> killedEnemies;  // Track killed enemies

    bool hasKilledEnemy(const std::string& enemyId) const;
    bool hasCollectedObject(const std::string& objectId) const;
};

#endif 
