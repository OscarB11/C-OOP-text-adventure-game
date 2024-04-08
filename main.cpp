#include <iostream>
#include <fstream>
#include <vector>
#include "Room.h"
#include "Player.h"
#include "json.hpp"
#include <algorithm>
#include <ctime>  
#include <cstdlib>  
using namespace std;

bool gameover = false;

using json = nlohmann::json;



// Enemy class definition
class Enemy {
public:
    std::string id;
    std::string desc;
    int aggressiveness;
    std::string initialRoom;
    std::vector<std::string> killedBy;

    bool isKilled(const std::vector<std::string>& killedEnemies) const;
};



class Object {
public:
    std::string id;
    std::string desc;
    std::string initialRoom;
    bool taken;  // Added to track whether the object is taken by the player

    // Constructor
    Object() : taken(false) {}

    // Function to get the object description
    std::string getDescription() const {
        return desc;
    }

    // Function to take the object by the player
    void take() {
        taken = true;
    }

    // Function to check if the object is taken by the player
    bool isTaken() const {
        return taken;
    }
};



// Objective class
class Objective {
public:
    std::string type;
    std::string targetEnemy;
    std::string targetObject;
    std::string targetRoom;
    std::vector<string> targets;


    // Constructor
    Objective() : type("none"), targetEnemy(""), targetObject(""), targetRoom("") {}

    // Function to check if the objective is achieved
    bool hasAchievedObjective(const Player& player, const vector<Enemy>& enemies, const vector<Object>& objects, const Room& currentRoom) const;
};

// Function to check the current objective
void checkObjective(const Objective& objective, const Player& player, const Room& currentRoom, const std::vector<Enemy>& enemies, bool& gameOver) {
    

    if (objective.type == "kill" && player.hasKilledEnemy(objective.targetEnemy)) {
        cout << objective.targetEnemy << " has been killed." << endl;
        cout << "Congratulations! You have completed the 'kill' objective and won the game." << endl;
        gameOver = true;
    } else if (objective.type == "reach" && currentRoom.id == objective.targetRoom) {
        cout << "Congratulations! You are in the required room and have completed the 'reach' objective." << endl;
        gameOver = true;
    } else if (objective.type == "collect" && find(player.inventory.begin(), player.inventory.end(), objective.targetObject) != player.inventory.end()) {
        cout << "Congratulations! You have collected the required item and completed the 'collect' objective." << endl;
        gameOver = true;
    }
}



bool killEnemy(const std::vector<Enemy>& enemies, const std::vector<Object>& playerObjects, const std::string& enemyID, std::vector<std::string>& killedEnemies) {
    // Find the enemy in the vector
    auto enemyIt = std::find_if(enemies.begin(), enemies.end(), [enemyID](const Enemy& enemy) {
        return enemy.id == enemyID;
    });

    // Check if the enemy was found
    if (enemyIt != enemies.end()) {
        // Check if the player has the required objects to kill the enemy
        const Enemy& enemy = *enemyIt;
        bool hasRequiredObjects = true;  // Assume the player has all required objects

        for (const std::string& requiredObject : enemy.killedBy) {
            auto objectIt = std::find_if(playerObjects.begin(), playerObjects.end(), [requiredObject](const Object& obj) {
                return obj.id == requiredObject && obj.isTaken();  // Check if the player has the object and it is taken
            });

            if (objectIt == playerObjects.end()) {
                // Player is missing a required object
                return false;
            }
        }
        
        if (hasRequiredObjects) {
            // Add the enemy to the list of killed enemies
            killedEnemies.push_back(enemyID);
            // Print a message for debugging
            
            return true;
        } else {
            // Player is missing required objects
            return false;
        }
    }

    // Enemy not found
    return false;
}


bool Player::hasKilledEnemy(const std::string& enemyId) const {
    for (const std::string& killedEnemy : killedEnemies) {
        if (killedEnemy == enemyId) {
            return true;
        }
    }
    return false;
}

bool Player::hasCollectedObject(const std::string& objectId) const {
    for (const std::string& collectedObject : inventory) {
        if (collectedObject == objectId) {
            return true;
        }
    }
    return false;
}



void combineAndPrint(const std::vector<Object>& objects,const std::vector<Enemy>& enemies, const std::string& currentRoom, const Room& room) {
    // Print the description of the current room
    cout << "-------------------\n";
    cout << "Description: " << room.desc << "\n";
    cout << "Exits: ";
    for (const auto& exit : room.exits) {
        cout << exit.first << " ";
    }
    cout << "\n-------------------";

    // Print objects in the room
    std::cout << "\nObjects in the room:\n";
    for (const Object& obj : objects) {
        if (obj.initialRoom == currentRoom && !obj.isTaken()) {
            std::cout << "- " << obj.id << ": " << obj.getDescription() ;
        }
    }

     // Print enemies in the room
    cout << "\n-------------------"; 
    std::cout << "\nEnemies in the room:\n";
    for (const Enemy& enemy : enemies) {
        if (enemy.initialRoom == currentRoom) {
            std::cout << "- " << enemy.id << ": " << enemy.desc;
            if (enemy.aggressiveness > 0) {
                std::cout << " (Aggression: " << enemy.aggressiveness << ")";
            }
            std::cout << "\n";
        }
    }


    cout << "\n-------------------\n";

}

// Function to print objects in the current room
void printObjectsInRoom(const std::vector<Object>& objects, const std::string& currentRoom) {
    std::cout << "\nObjects in the room:\n";
    for (const Object& obj : objects) {
        if (obj.initialRoom == currentRoom && !obj.isTaken()) {
            std::cout << "- " << obj.id << ": " << obj.getDescription() << "\n";
        }
    }
}

// Function to print the description of the current room
void printRoomDescription(const Room& room) {
    cout << "\n-------------------\n";
    cout << "Description: " << room.desc << "\n";
    cout << "Exits: ";
    for (const auto& exit : room.exits) {
        cout << exit.first << " ";
    }
    cout << "\n-------------------\n";
}

void printObjectDescription(const std::vector<Object>& objects, const std::string& objectName) {
    for (const Object& obj : objects) {
        if (obj.id == objectName) {
            cout << "-------------------\n";
            std::cout << "Description of " << obj.id << ": " << obj.getDescription() << "\n";
            cout << "-------------------\n";

            return;
        }
    }

    std::cout << "Object not found: " << objectName << "\n";
}

void displayPlayerInventory(const Player& player) {
    cout << "-------------------\n";
    std::cout << "Player's Inventory:\n";
    if (player.inventory.empty()) {
        std::cout << "Empty\n";
    } else {
        for (const std::string& item : player.inventory) {
            std::cout << "- " << item << "\n";
        }
    }
    cout << "-------------------\n";

}


void printIntroMenu() {
    std::cout << "\n\n*****************************************" << std::endl;
    std::cout << "*********** Text Adventure Game *********" << std::endl;
    std::cout << "************** Created by ****************" << std::endl;
    std::cout << "************ Best Oscar-osifo ************" << std::endl;
    std::cout << "*****************************************\n\n" << std::endl;
}

void printHelpPage() {
    std::cout << "\n\n*********************************************" << std::endl;
    std::cout << "************ Game Instructions *************" << std::endl;
    std::cout << "*******************************************\n" << std::endl;
    std::cout << "1. Use 'go xxx' to move to a different room." << std::endl;
    std::cout << "2. Use 'look xxx' to view information about an object." << std::endl;
    std::cout << "3. Use 'take xxx' or 'grab xxx' to pick up an object." << std::endl;
    std::cout << "4. Use 'list items' to see all the items you have picked." << std::endl;
    std::cout << "5. Use 'kill xxx' to defeat an enemy." << std::endl;
    std::cout << "6. Pay attention to the game objective to win!" << std::endl;
    std::cout << "7. Use 'quit' or 'exit' to quit the game." << std::endl;
    std::cout << "8. use look or look around to get description of current room" << std::endl;
    std::cout << "********************************************* \n\n" << std::endl;
}

void printCongratulatoryMessage() {
    std::cout << "\n\n*********************************************" << std::endl;
    std::cout << "*********** Congratulations! You Won! *********" << std::endl;
    std::cout << "*********************************************\n\n" << std::endl;
}


// Function to generate a random number between min and max (inclusive)
int generateRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// Function to load game data from a JSON file
void loadGame(const string& fileName, vector<Room>& rooms, vector<Object>& objects,
              vector<Enemy>& enemies, Player& player, Objective& objective) {
    ifstream fin(fileName);
    json j;
    fin >> j;

    // Load rooms
    for (const auto& roomData : j["rooms"]) {
        Room room;
        room.id = roomData["id"].get<string>();
        room.desc = roomData["desc"].get<string>();

        // Load exits
        for (const auto& exitData : roomData["exits"].items()) {
            room.exits[exitData.key()] = exitData.value();
        }

        rooms.push_back(room);
    }

    // Load objects
    for (const auto& objectData : j["objects"]) {
        Object object;
        object.id = objectData["id"].get<string>();
        object.desc = objectData["desc"].get<string>();
        object.initialRoom = objectData["initialroom"].get<string>();
        objects.push_back(object);
    }

    // Load enemies
    for (const auto& enemyData : j["enemies"]) {
        Enemy enemy;
        enemy.id = enemyData["id"].get<string>();
        enemy.desc = enemyData["desc"].get<string>();
        enemy.aggressiveness = enemyData["aggressiveness"].get<int>();
        enemy.initialRoom = enemyData["initialroom"].get<string>();

        // Load objects that can kill the enemy
        for (const auto& obj : enemyData["killedby"]) {
            enemy.killedBy.push_back(obj);
        }

        enemies.push_back(enemy);
    }

    // Load player
    player.initialRoom = j["player"]["initialroom"].get<string>();

    // Load objective

   

    objective.type = j["objective"]["type"].get<string>();
    cout << "Objective type is: " << objective.type << endl;

    // Load specific objective details based on type
    if (objective.type == "kill") {
        try {
            objective.targetEnemy = j["objective"]["targetenemy"].get<string>();
        } catch (const json::exception& e) {
            // Handle the case where the key or value is not present or is null
            // Provide a default value or perform appropriate error handling
            objective.targetEnemy = "default_target_enemy";
        }
        
    } else if (objective.type == "collect") {
        try {
            objective.targetObject = j["objective"]["targetobject"].get<string>();
        } catch (const json::exception& e) {
            // Handle the case where the key or value is not present or is null
            // Provide a default value or perform appropriate error handling
            objective.targetObject = "default_target_object";
    }
    } else if (objective.type == "reach") {
        try {
            objective.targetRoom = j["objective"]["targetroom"].get<string>();
        } catch (const json::exception& e) {
            // Handle the case where the key or value is not present or is null
            // Provide a default value or perform appropriate error handling
            objective.targetRoom = "default_target_room";
    }
}
    // Add additional logic for other objective types if needed
}




void startGame(const std::string& mapFileName) {
    vector<Room> rooms;
    vector<Object> objects;
    vector<Enemy> enemies;
    Player player;
    Objective objective;


    // Load game data
    loadGame(mapFileName, rooms, objects, enemies, player, objective);

    // Print intro menu
    printIntroMenu();

  
    Room currentRoom = rooms[0];


    std::string userInput;
    combineAndPrint(objects, enemies,currentRoom.id, currentRoom);

    while (true) {

        checkObjective(objective, player, currentRoom, enemies, gameover);
        
        if (gameover==true) {
            printCongratulatoryMessage();
            break;
            }

    // Print initial room description
       cout << "Enter your command:";
       cin >> std::ws; // Clear leading whitespace and newline characters
       std::getline(cin, userInput); // Use getline to read the whole line
       cout << "\n";


        // Process
        if (userInput == "help") {
            printHelpPage();

        } else if (userInput == "look"|| userInput == "look around") {
            // Implement 'look' command logic
            combineAndPrint(objects, enemies,currentRoom.id, currentRoom);
            cout << "\n";
         // Replace "currentRoom" with actual room ID

        } else if (userInput.find("look ")== 0) {
            // Implement 'look at' command logic
            std::string objectName = userInput.substr(5); // Extract the object name from the user input

            printObjectDescription(objects, objectName);
            cout << "\n";    

        } else if (userInput.find("take ") == 0|| userInput.find("grab ") == 0) {
            // Implement 'take' command logic
            std::string objectName = userInput.substr(5); // Extract object name after "take "
            // Check if the object is present in the room and not already taken
            bool objectFound = false;
            for (Object& obj : objects) {
                if (obj.id == objectName && obj.initialRoom == currentRoom.id && !obj.isTaken()) {
                    // Take the object
                    obj.take();
                    // Add the object to the player's inventory
                    player.inventory.push_back(objectName);
                    std::cout << "You have taken " << objectName << ".\n";
                    objectFound = true;
                    displayPlayerInventory(player);

                     
                    

                }
            }
            if (!objectFound) {
                cout << "Error: Object '" << objectName << "' not found or cannot be taken.\n";

            }
            cout << "\n";


        } else if (userInput.find("list items") == 0) {
            // Implement 'list items' command logic
            displayPlayerInventory(player);
            cout << "\n:";

        } else if (userInput.find("go ") == 0) {
            // Implement 'go' command logic
            std::string exitName = userInput.substr(3); // Extract exit name from the user input

           // Check if the exit exists in the current room
           auto exitIterator = currentRoom.exits.find(exitName);
           if (exitIterator != currentRoom.exits.end()) {
                // Check if there is an enemy in the current room
                auto enemyIterator = std::find_if(enemies.begin(), enemies.end(), [&currentRoom](const Enemy& enemy) {
                    return enemy.initialRoom == currentRoom.id;
                });

                if (enemyIterator != enemies.end()) {
                    // There is an enemy in the current room
                    const Enemy& enemy = *enemyIterator;
                    
                    
                    // Check if the player has killed the enemy
                    if (!player.hasKilledEnemy(enemy.id)) {
                        // The player has not killed the enemy
                        std::cout << "The enemy attacks you!\n";
                        
                        // Check if the enemy succeeds in attacking based on aggressiveness
                        int randomValue = generateRandomNumber(0, 100);
                        if (randomValue < enemy.aggressiveness) {
                            std::cout << "The enemy killed you. Game Over!\n";
                            return; // End the game
                        } else {
                            std::cout << "You escaped the attack!\n";
                            // Continue with the rest of the game logic
                        }
                    } else {
                        // The player has killed the enemy
                        std::cout << "You have killed the enemy. You can now leave the room.\n";
                    }
                }

                // Move to the room connected to the specified exit
                std::string nextRoomId = exitIterator->second;

                // Find the room with the nextRoomId
                auto nextRoomIterator = std::find_if(rooms.begin(), rooms.end(), [nextRoomId](const Room& room) {
                    return room.id == nextRoomId;
                });

                // Check if the room with the nextRoomId is found
                if (nextRoomIterator != rooms.end()) {
                    currentRoom = *nextRoomIterator;
                    combineAndPrint(objects,enemies, currentRoom.id, currentRoom);

                
                } else {
                    cout << "Error: Destination room not found.\n";
                }
            } else {
                cout << "Error: No exit named '" << exitName << "' in this room.\n"; 
            }
            cout << "\n";

            
        
        

        } else if (userInput.find("kill ") == 0) {
            // Implement game quitting logic
            // Extract the enemy name from the user input
            std::string enemyName = userInput.substr(5);

            // Find the enemy in the current room
            auto enemyIterator = std::find_if(enemies.begin(), enemies.end(), [enemyName, &currentRoom](const Enemy& enemy) {
                return enemy.id == enemyName && enemy.initialRoom == currentRoom.id;
            });

            //check if the enemy is present in the curent room
            if (enemyIterator != enemies.end()) {
                // Check if the player has the required objects to kill the enemy
                if (killEnemy(enemies, objects, enemyName, player.killedEnemies)) {
                    // Player successfully killed the enemy
                    std::cout << "You killed the enemy: " << enemyName << ".\n";
                    enemies.erase(enemyIterator); // Remove the killed enemy from the game

                
                } else {
                    // Player does not have the required objects to kill the enemy
                    std::cout << "You don't have the required objects to kill the enemy: " << enemyName << ".\n";
                    std::cout << "The enemy attacks you, and you died.\n";
                    // Implement game over logic and exit the game
                    // You can add more detailed game over messages and additional logic as needed
                    break;
                }
            } else {
                //Enemy not found in the current room
                std::cout << "Error: Enemy '" << enemyName << "' not found in this room.\n";
            }
            cout << "\n";

            
    

        } else if (userInput == "quit"|| userInput == "exit") {
            // Implement game quitting logic
            cout << "\n*****************************************" << endl;
            cout << " You quit thanks for playing! Goodbye!\n";
            cout << "*****************************************\n" << endl;
            break;  // Exit the game loop
        } else {
            cout << "\nInvalid command. Type 'help' for instructions.";
            cout << "\n";
        }

        
    }



 }





int main(int argc, char** argv) {
    printIntroMenu();

    std::string userInput;
    while (true) {
        std::cout << "Type 'help' for instructions or 'next' to pick map: ";
        std::cin >> userInput;
        cout << "\n";


        if (userInput == "help") {
            printHelpPage();
            cout << "\n";

        } else if (userInput == "next") {
            std::string mapFileName;
            std::cout << "Enter the map filename (e.g., map1.json): ";
            std::cin >> mapFileName;
            std::ifstream file(mapFileName);

            if (!file.good()) {
                std::cout << "Invalid map file. Try again." << std::endl;
                continue;
            }
            
            startGame(mapFileName);

        } else {
            std::cout << "Invalid input. Type 'help' or 'next'." << std::endl;
        }
    }

    return 0;
}


