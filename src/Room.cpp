#include <iostream>
#include <vector>
#include <string>
#include "Protocol.cpp"
using namespace std;

class Room {
private:
    string name;
    vector<int> clients_sock;
    vector<string> clients_names;

public:
    Room(const string& roomName) : name(roomName) {}

    void addClient(int client_socket, const string& username) {
        clients_sock.push_back(client_socket);
        clients_names.push_back(username);
        string welcome_message = "Welcome, " + username + "!\n";
        send(client_socket,welcome_message.c_str(), welcome_message.size(), 0);
        string join_msg = username + " has joined the room.";
        sendMsgToRoom(join_msg);
    }

    void removeClient(int client_socket, const string& username) {
        auto it = find(clients_sock.begin(), clients_sock.end(), client_socket);
        if (it != clients_sock.end()) {
            clients_sock.erase(it);
            string leave_msg = username + " has left the room.";
            sendMsgToRoom(leave_msg);
            close(client_socket);
        }
    }

    void sendMsgToRoom(const string& message) {
        string msg = message + "\n";
        for (int client_socket : clients_sock) {
            send(client_socket, msg.c_str(), msg.size(), 0);
        }
    }

    string getName() const {
        return name;
    }

    string getUserRegister(string u){
        auto it = find(clients_names.begin(), clients_names.end(), u);
        if(it != clients_names.end()){
            // Calcula la posición del socket en el vector
            int index = distance(clients_names.begin(), it);
            return clients_names[index];
        } else return "NO_SUCH_USER";
    }
};
