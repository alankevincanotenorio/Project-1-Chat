#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
#include "../libs/json.hpp"
#include "Message.cpp"
using json = nlohmann::json;
using namespace std;

class Room {
private:
    string name;
    unique_ptr<unordered_map<string, int>> clients;

public:
    Room(const string& roomName) : name(roomName), clients(make_unique<unordered_map<string, int>>()) {}

    void addClient(int client_socket, const string& success) {
        json response = StringToJSON(success);
        string username = response["extra"];
        (*clients)[username] = client_socket;
        json nu = makeIDENTIFY(NEW_USER, username);
        string m = JSONToString(nu);
        sendMsgToRoom(m);
    }

    void removeClient(int client_socket, const string& username) {
        auto it = clients->find(username);
        if (it != clients->end() && it->second == client_socket) {
            clients->erase(it);
            string leave_msg = username + " has left the room.";
            sendMsgToRoom(leave_msg);
            close(client_socket);
        }
    }

    void sendMsgToRoom(const string& message) {
        string msg;
        if(message.front() == '{' && message.back() == '}') msg = message;
        else msg = message + "\n";
        for (const auto& [username, client_socket] : *clients) {
            send(client_socket, msg.c_str(), msg.size(), 0);
        }
    }

    string getName() const {
        return name;
    }

    string getUserRegister(const string& user) const {
        auto it = clients->find(user);
        return (it != clients->end()) ? it->first : "NO_SUCH_USER";
    }
};