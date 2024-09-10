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
                user_name = json_msg["extra"];
                status = "\U0001F600";
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
            string s = json_msg["status"];
            if (s == "ACTIVE") status = "\U0001F600";
            if (s == "BUSY") status = "\U0001F623";
            if (s == "AWAY") status = "\U0001F914";
            string user_name = json_msg["username"];
            cout << "Nuevo estado actualizado: " << status << user_name << endl; 
        }  else if (message_type == "USER_LIST") {
            json users_json = json_msg["users"];
            cout << "Lista de usuarios en la sala:" << endl;
            for (auto& [username, status] : users_json.items()) {
                cout << username << ": " << status << endl;
            }
        } else if(message_type == "PUBLIC_TEXT_FROM"){
            string text = json_msg["text"];
            string n = json_msg["username"];
            cout << status << n << ": " << text << endl;
        }
        else {
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
            case USERS:
                json_msg = makeUSERS(type);
                break;
            case PUBLIC_TEXT:
                json_msg = makePbtext(type, message);
                break;
            default:
                cout <<"no mandaste un mensaje json";
                break;
        }
        string msg = JSONToString(json_msg);
        send(sock, msg.c_str(), msg.size(), 0);
        if (type == PUBLIC_TEXT) {
            cout << status << " " << user_name << ": " << message << endl;
        }
        cout << "Mensaje enviado json: " << msg << endl;
    }

    //maybe tener una enum
    void checkCommand(const string& input) {
        //id
        if (input.substr(0, 3) == "id ") {
            string user_name = input.substr(3);
            if (user_name.empty()) {
                cout << "No ingresaste el comando para identificarte" << endl;
                exit(0);
            }
            sendMessage(IDENTIFY, user_name);
        }
        //status
        else if (input.substr(0, 4) == "sts ") {
            string new_status = input.substr(4);
            if (new_status == "ACTIVE" || new_status == "AWAY" || new_status == "BUSY") {
                sendMessage(STATUS, new_status);
            } else {
                cout << "Estado inválido. Usa 'ACTIVE', 'AWAY' o 'BUSY'." << endl;
            }
        }
        //users list
        else if(input.substr(0, 6) == "users"){
            sendMessage(USERS, "");
        }
        //public message
        else if(input.substr(0, 3) == "pb "){
            string message = input.substr(3);
            if (message.empty()) {
                cout << "No ingresaste el mensaje" << endl;
                exit(0);
            }
            sendMessage(PUBLIC_TEXT, message);
        }
        else {
            cout << "Mensaje invalido"<< endl; //eliminar cuando implemente Disconnected
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