#include <iostream>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include "../libs/json.hpp"
#include "Room.cpp"
using namespace std;

class Server {
private:
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int port;
    unique_ptr<Room> generalRoom;

public:
    Server(int port) : port(port) {}

    //maybe erase if
    void initSocket() {
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
        generalRoom = make_unique<Room>("General");
    }

    //connect client method
    void connectClient() {
        cout << "Server waiting connections..." << endl;
        while (true) {
            int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            if (new_socket != -1) {
                thread clientThread(&Server::handleClient, this, new_socket);
                clientThread.detach();
            }
            cout << "Server waiting more connections..." << endl;
        }
    }

    //only left send a json to client
    void handleClient(int client_socket) {
        char buffer[512] = {0};
        if (userRegister(buffer, client_socket)) {
            string username = getData(buffer, "username");
            while (true) {
                int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
                if (bytes_read <= 0) break;
                buffer[bytes_read] = '\0';
                string message = getData(buffer, "text");
                if (message == "exit") {
                    generalRoom->removeClient(client_socket, username);
                    close(client_socket);
                    break;
                }
                string status = generalRoom->getStatus(username);
                if(status == "ACTIVE") status = "\U0001F600";
                generalRoom->sendMsgToRoom( status + username + ": " + message, client_socket);
            }  
        } else {
            json response = makeIDENTIFY(RESPONSE, getData(buffer, "username"), "USER_ALREADY_EXISTS");
            string usr_exist = JSONToString(response);
            send(client_socket, usr_exist.c_str(), usr_exist.size(), 0);
            close(client_socket);
        }

    }

    //verify is a user is registered
    bool userRegister(char username[], int client_socket){
        int bytes_read = read(client_socket, username, 512);
        username[bytes_read] = '\0';
        string u = getData(username, "username");
        if(u.size() > 8) {
            json r = makeINVALID(RESPONSE, "NOT_IDENTIFIED");
            string s = JSONToString(r);
            send(client_socket, s.c_str(), s.size(), 0);
            close(client_socket);
        }
        string n = generalRoom->getUserRegister(u);
        if (n != "NO_SUCH_USER") return false;
        else{
            json response = makeIDENTIFY(RESPONSE, u, "SUCCESS");
            string r = JSONToString(response);
            cout<<"El servidor responde: " << r;
            send(client_socket, r.c_str(), r.size(), 0);
            generalRoom->addClient(client_socket, r);
            return true;
        }
    }



    //modify
    ~Server() {
        if (server_fd != -1) {
            cout << "Server destroyed" << endl;
            if (close(server_fd) == -1) {
                cerr << "Error closing socket: " << strerror(errno) << endl;
            }
        }
    }

    int getPort() const {
        return port;
    }

    int getServer_fd() const {
        return server_fd;
    }

};