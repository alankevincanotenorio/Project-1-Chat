#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include "../libs/json.hpp"
#include "Message.cpp"
using json = nlohmann::json;
using namespace std;

/**
 * @brief Represents a chat room that manages connected clients and their interactions.
 */
class Room {
private:
    string name;

    /**
    * @brief Stores information about a connected client.
    */
    struct ClientData {
        string status;
        int socket_fd;
    };
    unique_ptr<unordered_map<string, ClientData>> clients;
    unordered_set<string> invited_users;

public:
    /**
     * @brief Constructor that initializes a room with a given name.
     * 
     * @param roomName The name of the room.
     */
    Room(const string& roomName) : name(roomName), clients(make_unique<unordered_map<string, ClientData>>()) {}

    /**
     * @brief Adds a new client to the room.
     * 
     * @param client_socket The socket file descriptor of the client.
     * @param username The username of the client.
     */
    void addNewClient(int client_socket, const string& username) {
        (*clients)[username] = {"ACTIVE", client_socket};
        json general_msg = makeIDENTIFY(NEW_USER, username);
        string msg_str = general_msg.dump();
        sendMsgToRoom(msg_str, client_socket);
    }

     /**
     * @brief Removes a client from the room.
     * 
     * @param client_socket The socket file descriptor of the client.
     * @param username The username of the client.
     */
    void removeClient(int client_socket, const string& username) {
        auto it = clients->find(username);
        if (it != clients->end() && it->second.socket_fd == client_socket) {
            json disconnected_msg = makeDISCONNECT(DISCONNECTED, "",username);
            string disconnected_str = disconnected_msg.dump();
            sendMsgToRoom(disconnected_str, client_socket);
            clients->erase(it);
            close(client_socket);
        }
    }

    /**
     * @brief Sends a message to all clients in the room, except the sender.
     * 
     * @param message The message to send.
     * @param socket_sender The socket file descriptor of the client sending the message.
     */
    void sendMsgToRoom(const string& message, int socket_sender) {
        string msg = message;
        for (const auto& [username, client_info] : *clients) {
            if (client_info.socket_fd != socket_sender) 
                send(client_info.socket_fd, msg.c_str(), msg.size(), 0);
        }
    }

    /**
     * @brief Updates the status of a client in the room.
     * 
     * @param username The username of the client.
     * @param new_status The new status to set for the client.
     */
    void updateStatus(const string& username, const string& new_status) {
        auto it = clients->find(username);
        if (it != clients->end()) {
            it->second.status = new_status;
            json status_msg = makeSTATUS(NEW_STATUS, new_status, username);
            string status_str = status_msg.dump();
            sendMsgToRoom(status_str, it->second.socket_fd);
        }
    }

    /**
     * @brief Sends the list of users in the room to a specific client.
     * 
     * @param client_socket The socket file descriptor of the client requesting the list.
     */
    void sendUserList(int client_socket) {
        unordered_map<string, string> users;
        for (const auto& [username, client_info] : *clients) {
            users[username] = client_info.status;
        }
        json user_list_msg = makeUSERS(USER_LIST, users);
        string user_list_str = user_list_msg.dump();
        send(client_socket, user_list_str.c_str(), user_list_str.size(), 0);
    }

    /**
     * @brief Checks if a user is in the room.
     * 
     * @param username The username to check.
     * @return True if the user is in the room, false otherwise.
     */
    bool isUserInRoom(const string& username) {
        return clients->find(username) != clients->end();
    }

    /**
     * @brief Gets the registration information of a user.
     * 
     * @param user The username to check.
     * @return The username if found, otherwise "NO_SUCH_USER".
     */
    string getUserRegister(const string& user) const {
        auto it = clients->find(user);
        return (it != clients->end()) ? it->first : "NO_SUCH_USER";
    }

    /**
     * @brief Gets the status of a user.
     * 
     * @param username The username whose status is requested.
     * @return The status of the user.
     */
    string getStatus(const string& username){
        return (*clients)[username].status;
    }

    /**
     * @brief Gets the socket file descriptor of a user.
     * 
     * @param username The username whose socket is requested.
     * @return The socket file descriptor if the user is found, otherwise -1.
     */
    int getUserSocket(const string& username) {
        auto it = clients->find(username);
        return (it != clients->end()) ? it->second.socket_fd : -1;
    }

    /**
     * @brief Gets the username associated with a given socket.
     * 
     * @param client_socket The socket file descriptor to check.
     * @return The username if found, otherwise "NO_SUCH_USER".
     */
    string getUsername(int client_socket) const {
        for (const auto& [username, client_info] : *clients) {
            if (client_info.socket_fd == client_socket) {
                return username;
            }
        }
        return "NO_SUCH_USER";
    }

    /**
     * @brief Gets a constant reference to the map of clients in the room.
     * 
     * @return A constant reference to the unordered map of clients.
     */
    const unordered_map<string, ClientData>& getClients() const {
        return *clients;
    }

    /**
     * @brief Adds a user to the list of invited users.
     * 
     * @param username The username of the user to invite.
     */
    void addUsersInvited(const string& username) {
        invited_users.insert(username);
    }

    /**
     * @brief Checks if a user has been invited to the room.
     * 
     * @param username The username to check.
     * @return True if the user has been invited, false otherwise.
     */
    bool isUserInvited(const string& username) const {
        return invited_users.find(username) != invited_users.end();
    }
};