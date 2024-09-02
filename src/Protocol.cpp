#include <iostream>
#include <string>
#include "../libs/json.hpp"
using json = nlohmann::json;
using namespace std;

//add rest of the types
enum MessageType{
    IDENTIFY,
    RESPONSE,
    NEW_USER,
    PUBLIC_TEXT_FROM
};

//make the others json
//String to json
json makeJSON(MessageType type, string message){
    json messageJSON;
    switch(type){
        case IDENTIFY:
            messageJSON["type"] = "IDENTIFY";
            messageJSON["username"] = message;
            break;
        case RESPONSE:
            messageJSON["type"] = "RESPONSE";
            messageJSON["operation"] = "IDENTIFY";
            messageJSON["result"] = "USER_ALREADY_EXISTS";
            messageJSON["extra"] = message;
            break;
        case NEW_USER:
            messageJSON["type"] = "NEW_USER";
            messageJSON["username"] = message;
            break;
        case PUBLIC_TEXT_FROM:
            messageJSON["type"] = "PUBLIC_TEXT_FROM";
            messageJSON["username"] = "Kimberly";
            messageJSON["text"] = message;
            break;
        default:
            break;
    }
    return messageJSON;
}

//serializar
string JSONToString(const json& j){
    string jsonMessage = j.dump();
    return jsonMessage;
}

//parsear
json StringToJSON(const string& jsonMessage){
    json json = json::parse(jsonMessage);
    return json;
}
