#include <iostream>
#include <unordered_map>
#include <unordered_set>
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
    unordered_set<string> invited_users;

public:
    //constructor
    Room(const string& roomName) : name(roomName), clients(make_unique<unordered_map<string, ClientData>>()) {}

    void addNewClient(int client_socket, const string& username) {
        (*clients)[username] = {"ACTIVE", client_socket};
        json general_msg = makeIDENTIFY(NEW_USER, username);
        string msg_str = general_msg.dump();
        sendMsgToRoom(msg_str, client_socket);
    }

    void removeClient(int client_socket, const string& username) {
        // Crear y enviar el mensaje LEFT_ROOM si el usuario está en un cuarto falta implementarlo
        auto it = clients->find(username);
        if (it != clients->end() && it->second.socket_fd == client_socket) {
            json disconnected_msg = makeDISCONNECT(DISCONNECTED, "",username);
            string disconnected_str = disconnected_msg.dump();
            sendMsgToRoom(disconnected_str, client_socket);
            clients->erase(it);
            close(client_socket);
        }
    }

    void sendMsgToRoom(const string& message, int socket_sender) {
        string msg = message;
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
            string status_str = status_msg.dump();
            sendMsgToRoom(status_str, it->second.socket_fd);
        }
    }

    void sendUserList(int client_socket) {
        unordered_map<string, string> users;
        for (const auto& [username, client_info] : *clients) {
            users[username] = client_info.status;
        }
        json user_list_msg = makeUSERS(USER_LIST, users);
        string user_list_str = user_list_msg.dump();
        send(client_socket, user_list_str.c_str(), user_list_str.size(), 0);
    }

    bool isUserInRoom(const string& username) {
        return clients->find(username) != clients->end();
    }

    string getUserRegister(const string& user) const {
        auto it = clients->find(user);
        return (it != clients->end()) ? it->first : "NO_SUCH_USER";
    }

    string getStatus(const string& username){
        return (*clients)[username].status;
    }

    string getName() const {
        return name;
    }

    int getUserSocket(const string& username) {
        auto it = clients->find(username);
        return (it != clients->end()) ? it->second.socket_fd : -1;
    }

    string getUsername(int client_socket) const {
        for (const auto& [username, client_info] : *clients) {
            if (client_info.socket_fd == client_socket) {
                return username;
            }
        }
        return "NO_SUCH_USER";
    }

    const unordered_map<string, ClientData>& getClients() const {
        return *clients;
    }


       // Agregar un usuario a la lista de invitados
    void addInvitee(const string& username) {
        invited_users.insert(username);
    }

    // Verificar si un usuario ya fue invitado
    bool isUserInvited(const string& username) const {
        return invited_users.find(username) != invited_users.end();
    }
};