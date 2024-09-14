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
    unordered_map<string, unique_ptr<Room>> rooms;

    /** 
     * @brief Handles client interactions, starting with identification and processing messages until the client disconnects.
     * 
     * @param client_socket The socket descriptor for the client.
     */
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

    /** 
     * @brief Identifies the client by reading the initial message to verify its identity.
     * 
     * @param client_socket The socket descriptor for the client.
     * @return True if the client is successfully identified; False otherwise.
     */    
    bool identifyClient(int client_socket) {
        char buffer[512] = {0};
        int bytes_read = read(client_socket, buffer, 512);
        cout << buffer << endl;
        if (bytes_read <= 0) return false;
        buffer[bytes_read] = '\0';
        json json_msg;
        if(!isValidJSON(buffer, json_msg)){
            sendErrorResponse(client_socket, "INVALID", "INVALID");
            return false;
        }
        if (json_msg.at("type") != "IDENTIFY") {
            sendErrorResponse(client_socket, "INVALID", "NOT_IDENTIFIED");
            return false;
        }
        return registerUser(json_msg, client_socket);
    }

    /** 
     * @brief Registers a new user in the general room after validating their username.
     * 
     * @param json_msg The JSON message containing the username.
     * @param client_socket The socket descriptor for the client.
     * @return True if the user is successfully registered; False otherwise.
     */
    bool registerUser(const json& json_msg, int client_socket) {
        string username = json_msg.at("username");
        if (username.size() > 8|| username.empty()) {
            json r = makeRESPONSE("INVALID", "NOT_IDENTIFIED");
            send(client_socket, r.dump().c_str(), r.dump().size(), 0);
            close(client_socket);
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
        generalRoom->addNewClient(client_socket, username);
        return true;
    }

    /** 
     * @brief Sends a message based on its type, either public, private, status update, etc.
     * 
     * @param buffer The raw message received from the client.
     * @param username The username of the client sending the message.
     * @param client_socket The socket descriptor for the client.
     */
    void sendMsg(char buffer[], const string& username, int client_socket) {  
        cout << "Mensaje recibido (raw): " << buffer << endl;  
        json json_msg;
        if(!isValidJSON(buffer, json_msg)){
            sendErrorResponse(client_socket, "INVALID", "INVALID");
            generalRoom->removeClient(client_socket, username);
            return;
        }
        string message_type = json_msg["type"];
        MessageType type = stringToMessageType(message_type);
        switch (type) {
            case PUBLIC_TEXT: {
                handlePublicText(json_msg, username, client_socket);
                break;
            }
            case STATUS: {
                handleStatusUpdate(json_msg, username, client_socket);
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
                handlePrivMessage(json_msg, username, client_socket);
                break;
            }
            case NEW_ROOM: {
                handleNewRoom(json_msg, username, client_socket);
                break;
            }
            case INVITE: {
                handleInvite(json_msg, username, client_socket);
                break;
            }
            default: {
                sendErrorResponse(client_socket, "INVALID", "INVALID");
                generalRoom->removeClient(client_socket, username);
                break;
            }
        }
    }
        
    /** 
     * @brief Sends a response message to a specific user.
     * 
     * @param target_user The username of the target recipient.
     * @param message The message to send to the user.
     */
    void sendResponseToUser(const string& target_user, const string& message) {
        int target_socket = generalRoom->getUserSocket(target_user);
        if (target_socket != -1) {
            send(target_socket, message.c_str(), message.size(), 0);
        }
    }
        
    /** 
     * @brief Checks if the provided buffer contains valid JSON.
     * 
     * @param buffer The buffer containing the message to parse.
     * @param json_msg Reference to a JSON object where parsed data will be stored.
     * @return True if the buffer contains valid JSON; False otherwise.
     */
    bool isValidJSON(const char* buffer, json& json_msg) {
        try {
            json_msg = json::parse(buffer);
            return true;
        } catch (json::parse_error& e) {
            return false;
        }
    }

    /** 
     * @brief Handles a public text message, validates it, and sends it to all users in the room.
     * 
     * @param json_msg The JSON message containing the text.
     * @param username The username of the client sending the message.
     * @param client_socket The socket descriptor for the client.
     */
    void handlePublicText(const json& json_msg, const string& username, int client_socket) {
        string message = json_msg["text"];
        if (message == "" || message == "exit") {
            if(message == "") sendErrorResponse(client_socket, "INVALID", "INVALID");
            generalRoom->removeClient(client_socket, username);
            close(client_socket);
            return;
        }
        json response = makePublictxt(PUBLIC_TEXT_FROM, message, username);
        generalRoom->sendMsgToRoom(response.dump(), client_socket);
    }

    /** 
     * @brief Handles a status update message, validates the new status, and updates the client's status.
     * 
     * @param json_msg The JSON message containing the new status.
     * @param username The username of the client updating the status.
     * @param client_socket The socket descriptor for the client.
     */
    void handleStatusUpdate(const json& json_msg, const string& username, int client_socket) {
        string new_status = json_msg["status"];
        if (new_status != "ACTIVE" && new_status != "AWAY" && new_status != "BUSY") {
            sendErrorResponse(client_socket, "INVALID", "INVALID");
            generalRoom->removeClient(client_socket, username);
            return;
        }
        generalRoom->updateStatus(username, new_status);
    }

    /** 
     * @brief Handles a private message to be sent to a specific user in the room.
     * 
     * @param json_msg The JSON message containing the recipient and message text.
     * @param username The username of the client sending the message.
     * @param client_socket The socket descriptor for the client.
     */
    void handlePrivMessage(const json& json_msg, const string& username, int client_socket) {
        string target_user = json_msg["username"];
        if (target_user.empty()) {
            json response = makeRESPONSE("INVALID", "INVALID");
            send(client_socket, response.dump().c_str(), response.dump().size(), 0);
            return;
        }
        string message = json_msg["text"];
        if (generalRoom->isUserInRoom(target_user)) {
            int target_socket = generalRoom->getUserSocket(target_user);
            if (target_socket == client_socket) {
                json response = makeRESPONSE("INVALID", "INVALID");
                send(client_socket, response.dump().c_str(), response.dump().size(), 0);
                return;
            }
            json response = makeTEXT(TEXT_FROM, message, username);
            sendResponseToUser(target_user, response.dump());
        } else {
            json response = makeTEXT(RESPONSE, "",target_user);
            send(client_socket, response.dump().c_str(), response.dump().size(), 0);
        }
    }

    /** 
     * @brief Handles the creation of a new room by a user, validates the room name, and registers the room.
     * 
     * @param json_msg The JSON message containing the room name.
     * @param username The username of the client creating the room.
     * @param client_socket The socket descriptor for the client.
     */
    void handleNewRoom(const json& json_msg, const string& username, int client_socket) {
        string roomname = json_msg["roomname"];
        if (roomname.size() > 16|| roomname.empty() || roomname.find(' ') != string::npos) {
            json r = makeRESPONSE("INVALID", "INVALID");
            send(client_socket, r.dump().c_str(), r.dump().size(), 0);
            close(client_socket);
            return;
        }
        if (rooms.find(roomname) != rooms.end()) {
            json response = makeNEWROOM(RESPONSE, roomname, "ROOM_ALREADY_EXISTS");
            send(client_socket, response.dump().c_str(), response.dump().size(), 0);
            return;
        }
        cout << "Creando la sala: " << roomname << " para el usuario: " << username << endl;
        rooms[roomname] = make_unique<Room>(roomname);
        json response = makeNEWROOM(RESPONSE, roomname, "SUCCESS");
        send(client_socket, response.dump().c_str(), response.dump().size(), 0);
        rooms[roomname]->addNewClient(client_socket, username);
        cout << "Salas actualmente en el servidor:" << endl;
        for (const auto& [room_name, room] : rooms) {
            cout << " - " << room_name << endl;
        }
    }

    /** 
     * @brief Handles inviting users to a specific room, validates invitees, and sends invitations.
     * 
     * @param json_msg The JSON message containing the room name and list of invitees.
     * @param username The username of the client sending the invitations.
     * @param client_socket The socket descriptor for the client.
     */
    void handleInvite(const json& json_msg, const string& username, int client_socket) {
        string roomname = json_msg["roomname"];
        if (roomname.empty() || json_msg["usernames"].empty()) {
            json response = makeRESPONSE("INVITE", "INVALID");
            send(client_socket, response.dump().c_str(), response.dump().size(), 0);
            return;
        }
        vector<string> invitees = json_msg["usernames"].get<vector<string>>();
        if (rooms.find(roomname) == rooms.end()) {
            json response = makeRESPONSE("INVITE", "NO_SUCH_ROOM", roomname);
            send(client_socket, response.dump().c_str(), response.dump().size(), 0);
            return;
        }
        if (!rooms[roomname]->isUserInRoom(username)) {
            json response = makeRESPONSE("INVALID", "INVALID");
            send(client_socket, response.dump().c_str(), response.dump().size(), 0);
            return;
        }
        for (const auto& invitee : invitees) {
            if (!generalRoom->isUserInRoom(invitee)) {
                json response = makeRESPONSE("INVITE", "NO_SUCH_USER", invitee);
                send(client_socket, response.dump().c_str(), response.dump().size(), 0);
                return;
            }
            if (rooms[roomname]->isUserInRoom(invitee) || rooms[roomname]->isUserInvited(invitee)) {
                cout << "Usuario " << invitee << " ya está en la sala " << roomname << ", ignorando invitación." << endl;
                continue;
            }
            rooms[roomname]->addUsersInvited(invitee);
            json invitation = jsonmakeINVITE(INVITATION, roomname, {}, username);
            int target_socket = generalRoom->getUserSocket(invitee);
            if (target_socket != -1) {
                send(target_socket, invitation.dump().c_str(), invitation.dump().size(), 0);
                cout << "Invitación enviada a " << invitee << " para unirse a la sala " << roomname << endl;
            }
        }
    }

    /** 
     * @brief Processes incoming messages from a client and routes them appropriately based on their type.
     * 
     * @param client_socket The socket descriptor for the client.
     * @param username The username of the client.
     * @return True if the message was processed successfully; False otherwise.
     */
    bool processClientMessage(int client_socket, const string& username) {
        char buffer[512] = {0};
        int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        cout << buffer << endl;
        if (bytes_read <= 0) return false;
        buffer[bytes_read] = '\0';
        sendMsg(buffer, username, client_socket);
        return true;
    }

    /** 
     * @brief Sends an error response to a client and closes the socket connection.
     * 
     * @param client_socket The socket descriptor for the client.
     * @param operation The type of operation that caused the error.
     * @param result The result message explaining the error.
     */
    void sendErrorResponse(int client_socket, const string& operation, const string& result) {
        json response = makeRESPONSE(operation, result);
        send(client_socket, response.dump().c_str(), response.dump().size(), 0);
        close(client_socket);
    }

public:
    string ipAddress;
    int port;

    /** 
     * @brief Constructs the server object with an IP address and port number, and initializes the general room.
     * 
     * @param ipAddress The IP address for the server to bind.
     * @param port The port number for the server to listen on.
     */
    Server(string ipAddress, int port) : ipAddress(ipAddress), port(port) {
        generalRoom = make_shared<Room>("General");
    }

    /** 
     * @brief Destructor that ensures the server and all client connections are closed.
     */
    ~Server() {
        if (server_fd != -1) {
            cout << "Closing server and all client connections..." << endl;
            for (const auto& [username, client_info] : generalRoom->getClients()) {
                close(client_info.socket_fd);
            }
            if (close(server_fd) == -1) {
                cerr << "Error closing server socket: " << strerror(errno) << endl;
            } else cout << "Server closed successfully" << endl;
        }
    }

    /** 
     * @brief Initializes the server socket, binds it to the IP and port, and prepares it to listen for incoming connections.
     */
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
    }

    /** 
     * @brief Accepts incoming client connections and creates a new thread for each client.
     */
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
};