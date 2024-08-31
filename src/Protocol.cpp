#include <iostream>
#include <string>
#include "../libs/json.hpp"
using json = nlohmann::json;
using namespace std;

enum MessageType{
    IDENTIFY,
    RESPONSE,
    NEW_USER,
    STATUS,
    NEW_STATUS,
    USERS,
    USER_LIST,
    TEXT,
    TEXT_FROM,
    PUBLIC_TEXT,
    PUBLIC_TEXT_FROM,
    NEW_ROOM,
    INVITE,
    INVITATION,
    JOIN_ROOM,
    JOINED_ROOM,
    ROOM_USERS,
    ROOM_USERS_LIST,
    ROOM_TEXT,
    ROOM_TEXT_FROM,
    LEAVE_ROOM,
    LEFT_ROOM,
    DISCONNECT,
    DISCONNECTED,
};

//falta hacer mas json
//funcion para pasar de string a json
json makeJSON(MessageType type, string message){
    json messageJSON;
    switch(type){
        case IDENTIFY:
        messageJSON["type"] = "IDENTIFY";
        messageJSON["username"] = message;
        break;
    }
    return messageJSON;
}

//im not so sure bro
string parseJSONToString(const json& j) {
    string result = "\"" + j.at("type").get<string>() + "\"\n";
    result += "\"" + j.at("username").get<string>() + "\"";
    return result;
}

int main(){
    cout<<"ingresa algo"<<endl;
    string message;
    getline(cin, message);
    json json = makeJSON(IDENTIFY, message);    
    string jj = parseJSONToString(json);
    cout<<jj<<endl;
    return 0;
}