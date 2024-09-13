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
    shared_ptr<Room> generalRoom;
    unique_ptr<unordered_map<string, unique_ptr<Room>>> rooms;

public:
    string ipAddress;
    int port;
    Server(string ipAddress, int port) : ipAddress(ipAddress), port(port) {}

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
        generalRoom = make_shared<Room>("General");
        // rooms = make_unique<unordered_map<string, unique_ptr<Room>>>();
        // rooms->emplace("General", generalRoom);
    }

    //connect client method
    void connectClient() {
        while (true) {
            cout << "Server waiting connections..." << endl;
            int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            if (new_socket != -1) {
                thread clientThread(&Server::handleClient, this, new_socket);
                clientThread.detach();
            }
        }
    }
    
    void handleClient(int client_socket) {
        if (!identifyClient(client_socket)) {
            close(client_socket);
            return;
        }
        string username = generalRoom->getUsername(client_socket);
        while (true) {
            if (!processClientMessage(client_socket, username)) {
                generalRoom->removeClient(client_socket, username);
                close(client_socket);
                break;
            }
        }
    }

    //aux
    bool identifyClient(int client_socket) {
        char buffer[512] = {0};
        int bytes_read = read(client_socket, buffer, 512);
        if (bytes_read <= 0) return false;
        buffer[bytes_read] = '\0';
        json json_msg;
        try {
            json_msg = json::parse(buffer);
        } catch (json::parse_error& e) {
            sendErrorResponse(client_socket, "INVALID", "INVALID");
            return false;
        }
        if (json_msg.at("type") != "IDENTIFY") {
            sendErrorResponse(client_socket, "INVALID", "NOT_IDENTIFIED");
            return false;
        }
        return registerUser(json_msg, client_socket);
    }

    // Función para procesar mensajes del cliente
    bool processClientMessage(int client_socket, const string& username) {
        char buffer[512] = {0};
        int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) return false;
        buffer[bytes_read] = '\0';
        sendMsg(buffer, username, client_socket);
        return true;
    }

    // Función para enviar un error y cerrar el socket
    void sendErrorResponse(int client_socket, const string& operation, const string& result) {
        json response = makeRESPONSE(operation, result);
        send(client_socket, response.dump().c_str(), response.dump().size(), 0);
        close(client_socket);
    }

    bool registerUser(const json& json_msg, int client_socket) {
        string username = json_msg.at("username");
        if (username.size() > 8|| username.empty()) {
            json r = makeRESPONSE("INVALID", "NOT_IDENTIFIED");
            sendResponseAndClose(client_socket, r.dump());
            return false;
        }
        string user = generalRoom->getUserRegister(username);
        if (user != "NO_SUCH_USER"){
            json response = makeIDENTIFY(RESPONSE, username, "USER_ALREADY_EXISTS");
            string usr_exist = response.dump();
            send(client_socket, usr_exist.c_str(), usr_exist.size(), 0);                
            return false;
        }
        json response = makeIDENTIFY(RESPONSE, username, "SUCCESS");
        send(client_socket, response.dump().c_str(), response.dump().size(), 0);
        generalRoom->addNewClient(client_socket, response.dump());
        return true;
    }

    //falta manejar el caso en public text que te manden un mensaje vacio
    //puede que lo vuelva a refactorizar para que sea menos largo y cumpla el P.R.U
    void sendMsg(char buffer[], const string& username, int client_socket) {    
        json json_msg;
        try{
            json_msg = json::parse(buffer);
        } catch (json::parse_error& e){
            json response = makeRESPONSE("INVALID", "INVALID");
            string r_str = response.dump();
            send(client_socket, r_str.c_str(), r_str.size(), 0);
            close(client_socket);
            generalRoom->removeClient(client_socket, username);
            return;
        }
        string message_type = json_msg["type"];
        MessageType type = stringToMessageType(message_type);
        switch (type) {
            case PUBLIC_TEXT: {
                string message = json_msg["text"];
                if (message == "exit" || message == "") { //el caso en que no te envien un mensaje si te desconecta pero no te avisa ni cierra terminal
                    generalRoom->removeClient(client_socket, username);
                    close(client_socket);
                    return;
                }
                json response = makePublictxt(PUBLIC_TEXT_FROM, message, username);
                generalRoom->sendMsgToRoom(response.dump(), client_socket);
                break;
            }
            case STATUS: {
                string new_status = json_msg["status"];
                if (new_status != "ACTIVE" && new_status != "AWAY" && new_status != "BUSY") {
                    json invalid_response = makeRESPONSE("INVALID", "INVALID");
                    sendResponseAndClose(client_socket, invalid_response.dump());
                    generalRoom->removeClient(client_socket, username);
                    return;
                }
                generalRoom->updateStatus(username, new_status);
                break;
            }
            case USERS: {
                generalRoom->sendUserList(client_socket);
                break;
            }
            case DISCONNECT: {
                generalRoom->removeClient(client_socket, username);
                break;
            }
            case TEXT: {
                string target_user = json_msg["username"];
                string message = json_msg["text"];
                if (generalRoom->isUserInRoom(target_user)) {
                    json response = makeTEXT(TEXT_FROM, message, username);
                    sendResponseToUser(target_user, response.dump());
                } else {
                    json response = makeTEXT(RESPONSE, target_user);
                    send(client_socket, response.dump().c_str(), response.dump().size(), 0);
                }
                break;
            }
            default: {
                json invalid_response = makeRESPONSE("INVALID", "INVALID");
                sendResponseAndClose(client_socket, invalid_response.dump());
                break;
            }
        }
    }

    // Función para enviar una respuesta y cerrar la conexión
    void sendResponseAndClose(int client_socket, const string& response) {
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
    }

    // Función para enviar un mensaje a un usuario específico
    void sendResponseToUser(const string& target_user, const string& message) {
        int target_socket = generalRoom->getUserSocket(target_user);
        if (target_socket != -1) {
            send(target_socket, message.c_str(), message.size(), 0);
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

};