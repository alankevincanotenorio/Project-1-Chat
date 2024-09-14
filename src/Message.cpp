#include <iostream>
#include <string>
#include "../libs/json.hpp"
using json = nlohmann::json;
using namespace std;

/**
 * @brief Enumeration of all types of messages that the protocol uses
 * NONE is not used in the protcol but is used for message with no type
 */
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

/**
 * @brief Converts a MessageType enum value to its string representation.
 * 
 * @param type The MessageType enum value to convert.
 * @return The string representation of the provided MessageType.
 */
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

/**
 * @brief Converts a string to its corresponding MessageType enum value.
 * 
 * @param type_str The string representation of the MessageType.
 * @return The MessageType enum value corresponding to the input string. Returns NONE if no match is found.
 */
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
    return NONE;
}

/**
 * @brief Creates a response message in JSON format.
 * 
 * @param operation The operation associated with the response.
 * @param result The result of the operation.
 * @param extra (Optional) additional information.
 * @return A JSON object representing the response message.
 */
json makeRESPONSE(const string& operation, const string& result, const string& extra = ""){
    json response;
    response["type"] = messageTypeToString(RESPONSE);
    response["operation"] = operation;
    response["result"] = result;
    if(operation == "INVALID") return response;
    response["extra"] = extra;
    return response;
}

/**
 * @brief Creates an identification message in JSON format.
 * 
 * @param type The type of identification message.
 * @param username The username associated with the identification.
 * @param result (Optional) result parameter used if the type is RESPONSE.
 * @return A JSON object representing the identification message.
 */
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

/**
 * @brief Creates a status message in JSON format.
 * 
 * @param type The type of status message.
 * @param status The status.
 * @param username (Optional) username associated with the status.
 * @return A JSON object representing the status message.
 */
json makeSTATUS(MessageType type, const string& status, const string& username = "") {
    json status_json;
    switch(type) {
        case STATUS:
            status_json["type"] = messageTypeToString(STATUS);
            status_json["status"] = status;
            break;
        case NEW_STATUS:
            status_json["type"] = messageTypeToString(NEW_STATUS);
            status_json["username"] = username;
            status_json["status"] = status;
            break;
        default:
            break;
    }
    return status_json;
}

/**
 * @brief Creates a user list or user list request message in JSON format.
 * 
 * @param type The type of users message.
 * @param users_map (Optional) map of usernames with their statuses.
 * @return A JSON object representing the users list message or user list request.
 */
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

/**
 * @brief Creates a public text message in JSON format.
 * 
 * @param type The type of public text message
 * @param message The content of the text message.
 * @param username (Optional) username associated with the message.
 * @return A JSON object representing the public text message.
 */
json makePublictxt(MessageType type, const string& message, const string& username = ""){
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

/**
 * @brief Creates a disconnect or room-leaving message in JSON format.
 * 
 * @param type The type of disconnect message
 * @param roomname (Optional) name of the room you are leaving
 * @param username (Optional) username associated with the disconnect case.
 * @return A JSON object representing the disconnect message.
 */
json makeDISCONNECT(MessageType type, const string& roomname = "", const string& username = "") {
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

/**
 * @brief Creates a text message in JSON format.
 * 
 * @param type The type of text message
 * @param message The content of the text message.
 * @param username (Optional) username associated with the message.
 * @return A JSON object representing the text message.
 */
json makeTEXT(MessageType type, const string& message, const string& username = "") {
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

/**
 * @brief Creates a new room message in JSON format.
 * 
 * @param type The type of room message.
 * @param roomname The name of the room to be created.
 * @param result (Optional) result used if the type is RESPONSE.
 * @return A JSON object representing the new room message.
 */
json makeNEWROOM(MessageType type, const string& roomname, const string& result = "") {
    json new_room;
    switch(type) {
        case NEW_ROOM:
            new_room["type"] = messageTypeToString(NEW_ROOM);
            new_room["roomname"] = roomname;
            break;
        case RESPONSE:
            new_room = makeRESPONSE(messageTypeToString(NEW_ROOM), result, roomname);
            break;
        default:
            break;
    }
    return new_room;
}

/**
 * @brief Creates an invitation message in JSON format.
 * 
 * @param type The type of invite message.
 * @param roomname The name of the room for the invitation.
 * @param usernames A list of usernames being invited.
 * @param username (Optional) username associated with the invitation, used if the type is INVITATION or RESPONSE.
 * @return A JSON object representing the invitation message.
 * 
 * @note In the case of INVITE, the list of usernames is used, but for INVITATION and RESPONSE, only the optional username parameter is relevant.
 */
json jsonmakeINVITE(MessageType type, const string& roomname, const vector<string>& usernames, const string& username = "") {
    json invitation;
    switch(type){
        case INVITE:
            invitation["type"] = messageTypeToString(INVITE);
            invitation["roomname"] = roomname;
            invitation["usernames"] = username;
            break;
        case INVITATION:
            invitation["type"] = messageTypeToString(INVITATION);
            invitation["username"] = username;
            invitation["roomname"] = roomname;
            break;
        case RESPONSE:
            invitation = makeRESPONSE(messageTypeToString(TEXT), "NO_SUCH_USER", username);
            break;
        default:
            break;
    }
    return invitation;
}