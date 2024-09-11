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
    NONE
};

string messageTypeToString(MessageType type) {
    switch (type) {
        case IDENTIFY: return "IDENTIFY";
        case RESPONSE: return "RESPONSE";
        case NEW_USER: return "NEW_USER";
        case STATUS: return "STATUS";
        case NEW_STATUS: return "NEW_STATUS";
        case USERS: return "USERS";
        case USER_LIST: return "USER_LIST";
        case TEXT: return "TEXT";
        case TEXT_FROM: return "TEXT_FROM";
        case PUBLIC_TEXT: return "PUBLIC_TEXT";
        case PUBLIC_TEXT_FROM: return "PUBLIC_TEXT_FROM";
        case NEW_ROOM: return "NEW_ROOM";
        case INVITE: return "INVITE";
        case INVITATION: return "INVITATION";
        case JOIN_ROOM: return "JOIN_ROOM";
        case JOINED_ROOM: return "JOINED_ROOM";
        case ROOM_USERS: return "ROOM_USERS";
        case ROOM_USERS_LIST: return "ROOM_USERS_LIST";
        case ROOM_TEXT: return "ROOM_TEXT";
        case ROOM_TEXT_FROM: return "ROOM_TEXT_FROM";
        case LEAVE_ROOM: return "LEAVE_ROOM";
        case LEFT_ROOM: return "LEFT_ROOM";
        case DISCONNECT: return "DISCONNECT";
        case DISCONNECTED: return "DISCONNECTED";
        default: return "hola";
    }
}

MessageType stringToMessageType(const string& type_str) {
    if (type_str == "IDENTIFY") return IDENTIFY;
    else if (type_str == "RESPONSE") return RESPONSE;
    else if (type_str == "NEW_USER") return NEW_USER;
    else if (type_str == "STATUS") return STATUS;
    else if (type_str == "NEW_STATUS") return NEW_STATUS;
    else if (type_str == "USERS") return USERS;
    else if (type_str == "USER_LIST") return USER_LIST;
    else if (type_str == "TEXT") return TEXT;
    else if (type_str == "TEXT_FROM") return TEXT_FROM;
    else if (type_str == "PUBLIC_TEXT") return PUBLIC_TEXT;
    else if (type_str == "PUBLIC_TEXT_FROM") return PUBLIC_TEXT_FROM;
    else if (type_str == "NEW_ROOM") return NEW_ROOM;
    else if (type_str == "INVITE") return INVITE;
    else if (type_str == "INVITATION") return INVITATION;
    else if (type_str == "JOIN_ROOM") return JOIN_ROOM;
    else if (type_str == "JOINED_ROOM") return JOINED_ROOM;
    else if (type_str == "ROOM_USERS") return ROOM_USERS;
    else if (type_str == "ROOM_USERS_LIST") return ROOM_USERS_LIST;
    else if (type_str == "ROOM_TEXT") return ROOM_TEXT;
    else if (type_str == "ROOM_TEXT_FROM") return ROOM_TEXT_FROM;
    else if (type_str == "LEAVE_ROOM") return LEAVE_ROOM;
    else if (type_str == "LEFT_ROOM") return LEFT_ROOM;
    else if (type_str == "DISCONNECT") return DISCONNECT;
    else if (type_str == "DISCONNECTED") return DISCONNECTED;
    return NONE;  // Tipo de mensaje desconocido
}



json makeRESPONSE(string operation, string result, string extra = ""){
    json response;
    response["type"] = messageTypeToString(RESPONSE);
    response["operation"] = operation;
    response["result"] = result;
    if(operation == "INVALID") return response;
    response["extra"] = extra;
    return response;
}

json makeIDENTIFY(MessageType type, const string& username, const string& result = "") {
    json id_json;
    switch(type) {
        case IDENTIFY:
            id_json["type"] = messageTypeToString(IDENTIFY);
            id_json["username"] = username;
            break;
        case NEW_USER:
            id_json["type"] = messageTypeToString(NEW_USER);
            id_json["username"] = username;
            break;
        case RESPONSE:
            id_json = makeRESPONSE(messageTypeToString(IDENTIFY), result, username);
            break;
        default:
            break;
    }
    return id_json;
}

json makeSTATUS(MessageType type, const string& result, const string& username = "") {
    json status;
    switch(type) {
        case STATUS:
            status["type"] = messageTypeToString(STATUS);
            status["status"] = result;
            break;
        case NEW_STATUS:
            status["type"] = messageTypeToString(NEW_STATUS);
            status["username"] = username;
            status["status"] = result;
            break;
        default:
            break;
    }
    return status;
}

json makeUSERS(MessageType type, const unordered_map<string, string>& users_map = {}) {
    json users_list;
    json users_json;
    switch(type) {
        case USERS:
            users_list["type"] = messageTypeToString(USERS);
            break;
        case USER_LIST:
            users_list["type"] = messageTypeToString(USER_LIST);
            for (const auto& [username, status] : users_map) {
                users_json[username] = status;
            }
            users_list["users"] = users_json;
            break;
        default:
            break;
    }
    return users_list;
}

json makePublictxt(MessageType type, string message, string username = ""){
    json public_txt;
    switch(type){
        case PUBLIC_TEXT:
            public_txt["type"] = messageTypeToString(PUBLIC_TEXT);
            public_txt["text"] = message;
            break;
        case PUBLIC_TEXT_FROM:
            public_txt["type"] = messageTypeToString(PUBLIC_TEXT_FROM);
            public_txt["username"] = username;
            public_txt["text"] = message;
            break;
        default:
            break;
    }
    return public_txt;
}

json makeDISCONNECT(MessageType type, string roomname = "", string username = "") {
    json disconnect;
    switch(type) {
        case DISCONNECT:
            disconnect["type"] = messageTypeToString(DISCONNECT);
            break;
        case DISCONNECTED:
            disconnect["type"] = messageTypeToString(DISCONNECTED);
            disconnect["username"] = username;
            break;
        case LEFT_ROOM:
            disconnect["type"] = messageTypeToString(LEFT_ROOM);
            disconnect["roomname"] = roomname;
            disconnect["username"] = username;
            break;
        default:
            break;
    }
    return disconnect;
}

json makeTEXT(MessageType type, string message, string username = ""){
    json text;
    switch(type){
        case TEXT:
            text["type"] = messageTypeToString(TEXT);
            text["username"] = username;
            text["text"] = message;
            break;
        case TEXT_FROM:
            text["type"] = messageTypeToString(TEXT_FROM);
            text["username"] = username;
            text["text"] = message;
            break;
        case RESPONSE:
            text = makeRESPONSE(messageTypeToString(TEXT), "NO_SUCH_USER", username);
            break;
        default:
            break;
    }
    return text;
}