#include <iostream>
#include <vector>
#include <string>
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
        // Opcional: enviar un mensaje a todos los clientes cuando un nuevo cliente se una
        string welcomeMessage = username + " has joined the room.";
        broadcastMessage(welcomeMessage);
    }

    void removeClient(int client_socket, const string& username) {
        auto it = find(clients_sock.begin(), clients_sock.end(), client_socket);
        if (it != clients_sock.end()) {
            clients_sock.erase(it);
            // Opcional: enviar un mensaje a todos los clientes cuando un cliente se desconecte
            string leaveMessage = username + " has left the room.";
            broadcastMessage(leaveMessage);
        }
    }

    void broadcastMessage(const string& message) {
        for (int client_socket : clients_sock) {
            send(client_socket, message.c_str(), message.size(), 0);
        }
    }

    string getName() const {
        return name;
    }
};
