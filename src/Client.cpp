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

    //chage to switch case
    void handleMessageType(const json& json_msg) {
        string message_type = json_msg["type"];
        if (message_type == "RESPONSE") {
            string result = json_msg["result"];
            if (result == "SUCCESS") {
                cout << "Te has registrado correctamente, ahora puedes enviar mensajes." << endl;
            } else if (result == "USER_ALREADY_EXISTS") {
                cout << "El nombre de usuario ya está en uso." << endl;
                close(sock);
                exit(0);
            } else if (result == "NOT_IDENTIFIED") {
                cout << "Sigue las reglas porfa" << endl;
                close(sock);
                exit(0);
            }
        } else if (message_type == "NEW_USER") {
            string user_name = json_msg["username"];
            cout << "Nuevo usuario conectado: " << status << user_name << endl; 

        } else if (message_type == "NEW_STATUS") {
            string user_name = json_msg["username"];
            cout << "Nuevo estado actualizado: " << status << user_name << endl; 
        } else {
            cout << "Tipo de mensaje no reconocido: " << message_type << endl;
        }
    }

    void receiveMessages() {
        char buffer[512] = {0};
        while (true) {
            int bytes_read = read(sock, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                string received(buffer);
                if (received.front() == '{' && received.back() == '}') {
                    cout << "Mensaje recibido JSON: " << received << endl;
                    json json_msg = StringToJSON(received);
                    handleMessageType(json_msg);
                } else {
                    cout << "Mensaje no JSON recibido: " << received;
                }
            } else if (bytes_read == 0) {
                close(sock);
                break;
            }
        }
    }

    void sendMessage(MessageType type, const string& message) {
        json json_msg;
        switch (type) {
            case IDENTIFY:
                json_msg = makeIDENTIFY(type, message);
                break;
            case STATUS:
                json_msg = makeSTATUS(type, message);
                break;
            default:
                json_msg = makeJSON(type, message);
                break;
        }
        string msg = JSONToString(json_msg);
        send(sock, msg.c_str(), msg.size(), 0);
        if (type == PUBLIC_TEXT_FROM) {
            cout << status << "Tú: " << message << endl;
        }
        cout << "Mensaje enviado json: " << msg << endl;
    }

    void checkCommand(const string& input) {
        if (input.substr(0, 3) == "id ") {
            string user_name = input.substr(3);
            if (user_name.empty()) {
                cout << "No ingresaste el comando para identificarte" << endl;
                exit(0);
            }
            sendMessage(IDENTIFY, user_name);
        }
        else if (input.substr(0, 4) == "sts ") {
            string new_status = input.substr(4);
            if (new_status == "ACTIVE" || new_status == "AWAY" || new_status == "BUSY") {
                sendMessage(STATUS, new_status);
            } else {
                cout << "Estado inválido. Usa 'ACTIVE', 'AWAY' o 'BUSY'." << endl;
            }
        }
        else {
            sendMessage(PUBLIC_TEXT_FROM, input);
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

};