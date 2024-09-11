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
    unique_ptr<Room> generalRoom;
    unique_ptr<unordered_map<int, int>> clients_sockets;
    unique_ptr<unordered_map<string, Room>> rooms;

public:
    string ipAddress;
    int port;
    Server(string ipAddress, int port) : ipAddress(ipAddress), port(port) {}

    //maybe erase if
    
    void initSocket() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            cerr << "Error creating socket: " << strerror(errno) << endl;
            return;
        }
        memset(&address, 0, sizeof(address));
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

    
    void handleClient(int client_socket) {
        char buffer[512] = {0};
        if (userRegister(buffer, client_socket)) {
            string username = getData(buffer, "username");
            while (true) {
                int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
                if (bytes_read <= 0) break;
                buffer[bytes_read] = '\0';
                sendMsg(buffer, username, client_socket);
            }
        } else {
            json response = makeIDENTIFY(RESPONSE, getData(buffer, "username"), "USER_ALREADY_EXISTS");
            string usr_exist = JSONToString(response);
            send(client_socket, usr_exist.c_str(), usr_exist.size(), 0);
            close(client_socket);
        }

    }

    //verify is a user is registered
    //falta el try-catch para la excepcion
    bool userRegister(char username[], int client_socket){
        int bytes_read = read(client_socket, username, 512);
        username[bytes_read] = '\0';
        string u = getData(username, "username");

        json json_msg = StringToJSON(username);
        
        if(u.size() > 8 || json_msg["type"] != "IDENTIFY" || u.size() == 0) {
            json r = makeINVALID(RESPONSE, "NOT_IDENTIFIED");
            string s = JSONToString(r);
            send(client_socket, s.c_str(), s.size(), 0);
            close(client_socket); 
            return false;
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

    //falta el try-catch para la excepcion
    void sendMsg(char buffer[], const string& username, int client_socket) {
        string msg(buffer);
        json json_msg = StringToJSON(msg);
        string message_type = json_msg["type"];
        if (message_type == "PUBLIC_TEXT") {
            string message = json_msg["text"];
            if (message == "exit" || message.size() == 0) {
                generalRoom->removeClient(client_socket, username);
                close(client_socket);
                return;
            }
            json response = makePbtext(PUBLIC_TEXT_FROM, message, username);
            string r = JSONToString(response);
            generalRoom->sendMsgToRoom(r, client_socket);
        } else if (message_type == "STATUS") {
            string new_status = json_msg["status"];
            if ((new_status != "ACTIVE" && new_status != "AWAY" && new_status != "BUSY") || (new_status.size() == 0)) {
                json invalid_response = makeINVALID(RESPONSE, "INVALID");
                string response_str = JSONToString(invalid_response);
                send(client_socket, response_str.c_str(), response_str.size(), 0);
                generalRoom->removeClient(client_socket, username);
                close(client_socket);
                return;
            }
            generalRoom->updateStatus(username, new_status);
        }  else if (message_type == "USERS") {
            generalRoom->sendUserList(client_socket);
        } else if (message_type == "DISCONNECT") {
            generalRoom->removeClient(client_socket, username);
        } else if(message_type == "TEXT"){
            string target_user = json_msg["username"];
            string message = json_msg["text"];
            if (generalRoom->isUserInRoom(target_user)) {
                json response = makeTEXT(TEXT_FROM, message, username);
                string response_str = JSONToString(response);
                int target_socket = generalRoom->getUserSocket(target_user);
                send(target_socket, response_str.c_str(), response_str.size(), 0);
            } else {
                json response = makeTEXT(RESPONSE, target_user);
                string response_str = JSONToString(response);
                send(client_socket, response_str.c_str(), response_str.size(), 0);
            }
        } //checar los mensajes invalidos
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