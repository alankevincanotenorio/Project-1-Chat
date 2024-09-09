#include <iostream>
#include <string>
#include "../libs/json.hpp"
using json = nlohmann::json;
using namespace std;

//all the types of message that the protocol use
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

//make the others json
//String to json
json makeJSON(MessageType type, string message){
    json id_json;
    switch(type){
        case IDENTIFY:
            id_json["type"] = "IDENTIFY";
            id_json["username"] = message;
            break;
        case RESPONSE:
            id_json["type"] = "RESPONSE";
            id_json["operation"] = "IDENTIFY";
            id_json["result"] = "USER_ALREADY_EXISTS";
            id_json["extra"] = message;
            break;
        case NEW_USER:
            id_json["type"] = "NEW_USER";
            id_json["username"] = message;
            break;
        case PUBLIC_TEXT_FROM:
            id_json["type"] = "PUBLIC_TEXT_FROM";
            id_json["username"] = "Kimberly";
            id_json["text"] = message;
            break;
        default:
            break;
    }
    return id_json;
}

json makeIDENTIFY(MessageType type, const string& username, const string& result = "") {
    json id_json;
    switch(type) {
        case IDENTIFY:
            id_json["type"] = "IDENTIFY";
            id_json["username"] = username;
            break;
        case NEW_USER:
            id_json["type"] = "NEW_USER";
            id_json["username"] = username;
            break;
        case RESPONSE: //maybe make method makeRESPONSE
            id_json["type"] = "RESPONSE";
            id_json["operation"] = "IDENTIFY";
            id_json["result"] = result;
            id_json["extra"] = username;
            break;
        default:
            break;
    }
    return id_json;
}

json makeINVALID(MessageType type, const string& result) {
    json id_json;
    id_json["type"] = "RESPONSE";
    id_json["operation"] = "INVALID";
    id_json["result"] = result;
    return id_json;
}

json makeSTATUS(MessageType type, const string& result, const string& username = "") {
    json id_json;
    switch(type) {
        case STATUS:
            id_json["type"] = "STATUS";
            id_json["status"] = result;
            break;
        case NEW_STATUS:
            id_json["type"] = "NEW_STATUS";
            id_json["username"] = username;
            id_json["status"] = result;
            break;
        default:
            cout << "Error: Tipo de mensaje de estado no soportado" << endl;
    }
    return id_json;
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


//get data from a json
    string getData(char buffer[], string type){
        string msg(buffer);
        json json_msg = StringToJSON(msg);
        string data;
        if(type == "username") data = json_msg["username"];
        if(type == "text") data = json_msg["text"];
        if(type == "result") data = json_msg["result"];
        return data;   
    }