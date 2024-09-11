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
    
    struct ClientData {
        string status;
        int socket_fd;
    };

    unique_ptr<unordered_map<string, ClientData>> clients;
    

public:
    Room(const string& roomName) : name(roomName), clients(make_unique<unordered_map<string, ClientData>>()) {}

    void addClient(int client_socket, const string& success) {
        json response = json::parse(success);
        //json response = StringToJSON(success);
        string username = response["extra"];
        (*clients)[username] = {"ACTIVE", client_socket};
        json nu = makeIDENTIFY(NEW_USER, username);
        string m = nu.dump();
        //string m = JSONToString(nu);
        sendMsgToRoom(m, client_socket);
    }

    void removeClient(int client_socket, const string& username) {
    // Crear y enviar el mensaje LEFT_ROOM si el usuario está en un cuarto falta implementarlo
    auto it = clients->find(username);
    if (it != clients->end() && it->second.socket_fd == client_socket) {
        clients->erase(it);
        json disconnected_msg = makeDISCONNECT(DISCONNECTED, "", username);
        string disconnected_str = disconnected_msg.dump();
        // string disconnected_str = JSONToString(disconnected_msg);
        sendMsgToRoom(disconnected_str, client_socket);
        close(client_socket);
    }
}


    void sendMsgToRoom(const string& message, int socket_sender) {
        string msg;
        if(message.front() == '{' && message.back() == '}') msg = message;
        else msg = message + "\n";
        for (const auto& [username, client_info] : *clients) {
            if (client_info.socket_fd != socket_sender) 
                send(client_info.socket_fd, msg.c_str(), msg.size(), 0);
        }
    }

    void updateStatus(const string& username, const string& new_status) {
        auto it = clients->find(username);
        if (it != clients->end()) {
            it->second.status = new_status;
            json status_msg = makeSTATUS(NEW_STATUS, new_status, username);
            string msg = status_msg.dump();
            // string msg = JSONToString(status_msg);
            sendMsgToRoom(msg, it->second.socket_fd);
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
        return (*clients)[username].status;
    }

    void sendUserList(int client_socket) {
        unordered_map<string, string> users;
        for (const auto& [username, client_info] : *clients) {
            users[username] = client_info.status;
        }
        json user_list_msg = makeUSERS(USER_LIST, users);
        string user_list_str = user_list_msg.dump();
        // string user_list_str = JSONToString(user_list_msg);
        send(client_socket, user_list_str.c_str(), user_list_str.size(), 0);
    }


    bool isUserInRoom(const string& username) {
        return clients->find(username) != clients->end();
    }


    int getUserSocket(const string& username) {
        auto it = clients->find(username);
        if (it != clients->end()) {
            return it->second.socket_fd;
        }
        return -1;
    }


};