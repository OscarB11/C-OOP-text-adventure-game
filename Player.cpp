// Player.cpp
#include "Player.h"



// Function to check if the player has killed a specific enemy
bool Player::hasKilledEnemy(const std::string& enemyId) const {
    for (const std::string& killedEnemy : killedEnemies) {
        // Check if the enemyId matches any killed enemy
        if (killedEnemy == enemyId) {
            return true;  
        }
    }
    return false;  
}


// Function to check if the player has collected a specific object
bool Player::hasCollectedObject(const std::string& objectId) const {
    for (const std::string& collectedObject : inventory) {
        // Check if the objectId matches any collected object
        if (collectedObject == objectId) {
            return true;  // Player has collected this object
        }
    }
    return false;  
}


