#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
#include "Protocol.cpp"
using namespace std;

class Room {
private:
    string name;
    unique_ptr<unordered_map<string, int>> clients;

public:
    Room(const string& roomName) : name(roomName), clients(make_unique<unordered_map<string, int>>()) {}

    void addClient(int client_socket, const string& username) {
        (*clients)[username] = client_socket;
        string welcome_message = "Welcome, " + username + "!\n";
        send(client_socket,welcome_message.c_str(), welcome_message.size(), 0);
        string join_msg = username + " has joined the room.";
        sendMsgToRoom(join_msg);
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
        string msg = message + "\n";
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