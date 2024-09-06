#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
#include "../libs/json.hpp"
#include "Message.cpp"
#include <tuple>
using json = nlohmann::json;
using namespace std;

class Room {
private:
    string name;
    unique_ptr<unordered_map<string, tuple<string, int>>> clients;

public:
    Room(const string& roomName) : name(roomName), clients(make_unique<unordered_map<string, tuple<string, int>>>()) {}

    void addClient(int client_socket, const string& success) {
        json response = StringToJSON(success);
        string username = response["extra"];
        (*clients)[username] = make_tuple("ACTIVE", client_socket);
        json nu = makeIDENTIFY(NEW_USER, username);
        string m = JSONToString(nu);
        sendMsgToRoom(m, client_socket);
        // string status = get<0>((*clients)[username]);
        // send(client_socket, status.c_str(), status.size(), 0);
    }

    void removeClient(int client_socket, const string& username) {
        auto it = clients->find(username);
        if (it != clients->end() && get<1>(it->second) == client_socket) {
            clients->erase(it);
            string leave_msg = username + " has left the room.";
            sendMsgToRoom(leave_msg, client_socket);
            close(client_socket);
        }
    }

    void sendMsgToRoom(const string& message, int socket_sender) {
        string msg;
        if(message.front() == '{' && message.back() == '}') msg = message; //por esta linea falla, primero tendre que procesar un json y luego otro
        else msg = message + "\n";
        for (const auto& [username, client_info] : *clients) {
            int client_socket = get<1>(client_info);
            if (client_socket != socket_sender) send(client_socket, msg.c_str(), msg.size(), 0);
        }
    }

    string getName() const {
        return name;
    }

    string getUserRegister(const string& user) const {
        auto it = clients->find(user);
        return (it != clients->end()) ? it->first : "NO_SUCH_USER";
    }

    string getStatus(const string& username){
        return get<0>((*clients)[username]);
    }
};