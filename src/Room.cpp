#include <iostream>
#include <string>
#include "../libs/json.hpp"
#include <memory>
using json = nlohmann::json;
using namespace std;

class Room {
    string room_name;
    json userList;
public:
    Room(string room_name) : room_name(room_name) {}

    Room createRoom(string name){
        return Room(name);
    }

    void join(string username){
        userList[username] = "Active";
    }

};