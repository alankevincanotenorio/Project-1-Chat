#include <iostream>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include "../libs/json.hpp"
#include "Room.cpp"
using json = nlohmann::json;
using namespace std;

class Server {
private:
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int port;
    bool socket_open;
    json users;
    vector<int> clients_sock;
    bool client_connected = false;
    unique_ptr<Room> generalRoom;

public:
    Server(int port) : port(port), socket_open(false){}

    void initSocket() {
        if (socket_open) {
            cerr << "The main socket is open" << endl;
            return;
        }
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            cerr << "Error creating socket: " << strerror(errno) << endl;
            return;
        }
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
            cerr << "Error configuring socket: " << strerror(errno) << endl;
            close(server_fd);
            return;
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            cerr << "Error using bind: " << strerror(errno) << endl;
            close(server_fd);
            return;
        }
        if (listen(server_fd, SOMAXCONN) < 0) {
            cerr << "Error listen: " << strerror(errno) << endl;
            close(server_fd);
            return;
        }
        socket_open = true;
        generalRoom = make_unique<Room>("General");
    }

    void connectClient() {
        cout << "Server waiting connections..." << endl;
        while (true) {
            int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            if (new_socket != -1) {
                clients_sock.push_back(new_socket);
                char buffer[512] = {0};
                read(new_socket, buffer, 512);
                string username(buffer);
                if (getUserRegister(username) != "NO_SUCH_USER") {
                    cout<<"User registered"<<endl;
                    close(new_socket);
                } else{
                    addUser(username);
                    generalRoom->addClient(new_socket, username);
                    thread clientThread(&Server::handleClient, this, new_socket, username);
                    clientThread.detach();
                }
            }
            cout << "Server waiting more connections..." << endl;
        }
    }

    void handleClient(int client_socket, string username) {
        char buffer[512] = {0};
        while (true) {
            int bytes_read = read(client_socket, buffer, 512);
            if (bytes_read <= 0) {
                break;
            }
            string message(buffer, bytes_read);
            generalRoom->sendMsgToRoom(username + ": " + message);
            buffer[0] = '\0';
        }
        auto it = find(clients_sock.begin(), clients_sock.end(), client_socket);
        if (it != clients_sock.end()) {
            clients_sock.erase(it);
        }
        close(client_socket);
        generalRoom->removeClient(client_socket, username);
    }

    ~Server() {
        if (socket_open && server_fd != -1) {
            cout << "Server destroyed" << endl;
            if (close(server_fd) == -1) {
                cerr << "Error closing socket: " << strerror(errno) << endl;
            } else {
                socket_open = false; 
            }
        }
        for (int client_socket : clients_sock) {
            close(client_socket);
        }
    }

    int getPort() const {
        return port;
    }

    int getServer_fd() const {
        return server_fd;
    }

    bool getSocket_open() {
        return socket_open;
    }

    string getUserRegister(string user) {
        if (users.find(user) != users.end()) return user;
        else return "NO_SUCH_USER";
    }

    bool getClientConnected() {
        return client_connected;
    }

    void addUser(string username) {
        users[username] = "ACTIVE";
    }
};