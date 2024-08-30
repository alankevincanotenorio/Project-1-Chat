#include <iostream>
#include <string>
#include "../libs/json.hpp"
// #include "Room.cpp"
using json = nlohmann::json;
using namespace std;

enum MessageType{
    IDENTIFY,
    RESPONSE,
    NEW_USER,
    TEXT_FROM
};


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