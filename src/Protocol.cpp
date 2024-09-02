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

//im not so sure bro
string parseJSONToString(const json& j) {
    string result = "\"" + j.at("type").get<string>() + "\"\n";
    result += "\"" + j.at("username").get<string>() + "\"";
    return result;
}




// int main() {
//     std::string json_string = R"({"name": "John", "age": 30})";

//     // Parsear el string JSON a un objeto JSON
//     json j = json::parse(json_string);

//     // Acceder a los datos
//     std::string name = j["name"];
//     int age = j["age"];

//     std::cout << "Name: " << name << std::endl;
//     std::cout << "Age: " << age << std::endl;

//     return 0;
// }
