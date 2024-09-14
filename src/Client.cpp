#include <iostream>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include "Message.cpp"
using namespace std;

class Client {
private:
    int sock = 0;
    struct sockaddr_in serv_addr;
    string server_ip;
    int server_port;
    unique_ptr <thread> receiveThread;
    bool is_identified = false;

    void handleMessageType(const json& json_msg) {
        string message_type_str = json_msg["type"];
        MessageType message_type = stringToMessageType(message_type_str);
        switch (message_type) {
            case RESPONSE:
                handleResponse(json_msg);
                break;
            case NEW_USER:
                handleNewUser(json_msg);
                break;
            case NEW_STATUS:
                handleStatusUpdate(json_msg);
                break;
            case USER_LIST:
                handleUserList(json_msg);
                break;
            case TEXT_FROM:
                handlePrivateMessage(json_msg);
                break;
            case PUBLIC_TEXT_FROM:
                handlePublicMessage(json_msg);
                break;
            case DISCONNECTED:
                handleDisconnection(json_msg);
                break;
            case INVITATION:
                handleInvitation(json_msg);
                break;
            default:
                cout << "Unrecognized message: " << message_type_str << endl;
                break;
        }
    }

    void handleResponse(const json& json_msg) {
        string result = json_msg["result"];
        string operation = json_msg["operation"];
        if (result == "SUCCESS") {
            if(operation == "NEW_ROOM"){
                cout << "The room " << json_msg["extra"] << " has been created." << endl;
            }
            else{
                is_identified = true;
                user_name = json_msg["extra"];
                cout << "Welcome! " << user_name << " ,now you can send messages." << endl;
            }
        } else if (result == "USER_ALREADY_EXISTS") {
            cout << "Sorry, the username is used by another user." << endl;
            close(sock);
            exit(0);
        } else if (result == "NOT_IDENTIFIED") {
            cout << "You have not been identified because you did not enter the correct command or you entered an empty username." << endl;
            close(sock);
            exit(0);
        } else if (result == "INVALID") {
            cout << "Invalid message." << endl;
            close(sock);
            exit(0);
        } else if (result == "NO_SUCH_USER") {
            cout << "The user '" << json_msg["extra"] << "' does not exist." << endl;
        } else if (result == "ROOM_ALREADY_EXIST") {
            cout << "The room '" << json_msg["extra"] << "' already exist." << endl;
            close(sock);
            exit(0);
        } else if (result == "NO_SUCH_ROOM") {
            string roomname = json_msg["extra"];
            cout << roomname << " does not exist" << endl;
        }
    }

    void handleNewUser(const json& json_msg) {
        string new_user = json_msg["username"];
        cout << new_user << " has been connected." << endl;
    }

    void handleStatusUpdate(const json& json_msg) {
        string new_status = json_msg["status"];
        string username = json_msg["username"];
        cout << username << " updated its status to: " << new_status << endl;
    }

    void handleUserList(const json& json_msg) {
        json users_json = json_msg["users"];
        cout << "Users in this room:" << endl;
        for (auto& [username, status] : users_json.items()) {
            cout << username << ": " << status << endl;
        }
    }

    void handlePrivateMessage(const json& json_msg) {
        string text = json_msg["text"];
        string from_user = json_msg["username"];
        cout << "Private message from " << from_user << ": " << text << endl;
    }

    void handlePublicMessage(const json& json_msg) {
        string text = json_msg["text"];
        string username = json_msg["username"];
        cout << username << ": " << text << endl;
    }

    void handleDisconnection(const json& json_msg) {
        string username = json_msg["username"];
        cout << username << " has been disconnected." << endl;
    }

    void handleInvitation(const json& json_msg) {
        string roomname = json_msg["roomname"];
        string username = json_msg["username"];
        cout << username << " invited  you to join to the room " << roomname << endl;
    }

    //readMessages
    void receiveMessages() {
        char buffer[512] = {0};
        while (true) {
            int bytes_read = read(sock, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                string received(buffer);
                if (received.front() == '{' && received.back() == '}') {
                    cout << "Mensaje recibido JSON: " << received << endl;
                    json json_msg = json::parse(received);
                    handleMessageType(json_msg);
                }
            } else if (bytes_read == 0) {
                close(sock);
                break;
            }
        }
    }

    void sendMessage(MessageType type, const string& message, const string& target_user = "") {
        json json_msg;
        switch (type) {
            case IDENTIFY:
                json_msg = makeIDENTIFY(type, message);
                break;
            case STATUS:
                json_msg = makeSTATUS(type, message);
                break;
            case USERS:
                json_msg = makeUSERS(type);
                break;
            case PUBLIC_TEXT:
                json_msg = makePublictxt(type, message);
                break;
            case DISCONNECT:
                json_msg = makeDISCONNECT(type);
                break;
            case TEXT:
                json_msg = makeTEXT(type, message, target_user);
                break;
            case NEW_ROOM:
                json_msg = makeNEWROOM(type, message);
                break; //agregar invite
            default:
                break;
        }
        string msg = json_msg.dump();
        if(type != NONE) {
            send(sock, msg.c_str(), msg.size(), 0);
            if (type == PUBLIC_TEXT) {
                cout << user_name << ": " << message << endl;
            }
            cout << "Mensaje enviado json: " << msg << endl;
        } else{
            send(sock, message.c_str(), message.size(), 0);
            cout << "Mensaje no json: " << message << endl;
        } 
    }

    //maybe tener una enum o una clase commands
    void checkCommand(const string& input, string username = "") {
        //id
        if (input.substr(0, 3) == "id ") {
            string user_name = input.substr(3);
            sendMessage(IDENTIFY, user_name);
        } else{
            //status
            if (input.substr(0, 4) == "sts ") {
                string new_status = input.substr(4);
                sendMessage(STATUS, new_status);
            }
            //users list
            else if(input.substr(0, 6) == "users"){
                sendMessage(USERS, "");
            }
            //disconnect
            else if (input == "exit") {
                sendMessage(DISCONNECT, "");
                close(sock);
                cout << "Te has desconectado del servidor." << endl;
                exit(0);
            }
            //public message
            else if (input.substr(0, 3) == "pb ") {
                string message = input.substr(3);            
                sendMessage(PUBLIC_TEXT, message);
            } 
            // private text
            else if (input.substr(0, 4) == "txt ") {
                size_t spacePos = input.find(" ", 4);
                string target_user = input.substr(4, spacePos - 4);
                string private_message = input.substr(spacePos + 1);
                sendMessage(TEXT, private_message, target_user);
            } 
            //make rooms
            else if (input.substr(0, 7) == "mkRoom ") {
                string  room_name = input.substr(7);
                sendMessage(NEW_ROOM, room_name);
            }
            // invite
            else if (input.substr(0, 5) == "invt ") {
                size_t spacePos = input.find(" ", 5);
                if (spacePos != string::npos) {
                    string room_name = input.substr(5, spacePos - 5);
                    string usernames = input.substr(spacePos + 1);
                    vector<string> users_invit;
                    size_t pos = 0;
                    while ((pos = usernames.find(",")) != string::npos) {
                        users_invit.push_back(usernames.substr(0, pos));
                        usernames.erase(0, pos + 1);
                    }
                    users_invit.push_back(usernames);
                    json json_msg;
                    json_msg["type"] = messageTypeToString(INVITE);
                    json_msg["roomname"] = room_name;
                    json_msg["usernames"] = users_invit;
                    send(sock, json_msg.dump().c_str(), json_msg.dump().size(), 0);
                    cout << "Mensaje enviado json: " << json_msg.dump() << endl;
                }
            } 
            else{
                sendMessage(NONE,input);
            }
        }
    }

public:
    string user_name;
    string status;

    Client(const string &server_ip, int port) : server_ip(server_ip), server_port(port) {}

    int connectToServer() {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(server_port);
        inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr);
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
            cout << "Server not connected" << endl;
            return -1;
        }
        thread receiveThread(&Client::receiveMessages, this);
        receiveThread.detach();
        return 0;
    }

    void connection() {
        cout << "Hello, please insert your username (max. 8 characters)" << endl;
        string input;
        getline(cin, input);
        checkCommand(input);
        while (true) {
            getline(cin, input);
            checkCommand(input);
        }
    }

    ~Client() {
        if (sock != -1) {
            cout << "Closing client connection..." << endl;
            if (is_identified) {
                sendMessage(DISCONNECT, "");
            }
            close(sock);
            cout << "Socket closed." << endl;
        }
    }

};