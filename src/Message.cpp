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
    NONE //no estoy seguro
};

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

json makeUSERS(MessageType type, const unordered_map<string, string>& users = {}) {
    json id_json;
    json users_json;
    switch(type) {
        case USERS:
            id_json["type"] = "USERS";
            break;
        case USER_LIST:
            id_json["type"] = "USER_LIST";
            for (const auto& [username, status] : users) {
                users_json[username] = status;
            }
            id_json["users"] = users_json;
            break;
        default:
            cout << "Error: Tipo de mensaje no soportado" << endl;
            break;
    }
    return id_json;
}

json makePbtext(MessageType type, string message, string username = ""){
    json id_json;
    switch(type){
        case PUBLIC_TEXT:
            id_json["type"] = "PUBLIC_TEXT";
            id_json["text"] = message;
            break;
        case PUBLIC_TEXT_FROM:
            id_json["type"] = "PUBLIC_TEXT_FROM";
            id_json["username"] = username;
            id_json["text"] = message;
            break;
        default:
            cout << "Error: Tipo de mensaje no soportado" << endl;
            break;
    }
    return id_json;
}


json makeDISCONNECT(MessageType type, string roomname = "", string username = "") {
    json id_json;
    switch(type) {
        case DISCONNECT:
            id_json["type"] = "DISCONNECT";
            break;
        case DISCONNECTED:
            id_json["type"] = "DISCONNECTED";
            id_json["username"] = username;
            break;
        case LEFT_ROOM:
            id_json["type"] = "LEFT_ROOM";
            id_json["roomname"] = roomname;
            id_json["username"] = username;
            break;
        default:
            cout << "Error: Tipo de mensaje de estado no soportado" << endl;
    }
    return id_json;
}

json makeTEXT(MessageType type, string message, string username = ""){
    json id_json;
    switch(type){
        case TEXT:
            id_json["type"] = "TEXT";
            id_json["username"] = username;
            id_json["text"] = message;
            break;
        case TEXT_FROM:
            id_json["type"] = "TEXT_FROM";
            id_json["username"] = username;
            id_json["text"] = message;
            break;
        case RESPONSE: //maybe make method makeRESPONSE
            id_json["type"] = "RESPONSE";
            id_json["operation"] = "IDENTIFY";
            id_json["result"] = "NO_SUCH_USER";
            id_json["extra"] = username;
            break;
        default:
            cout << "Error: Tipo de mensaje no soportado" << endl;
            break;
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