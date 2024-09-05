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
    string ip;
    int server_port;
    unique_ptr <thread> receiveThread;

    //falta agregar que el username esta delimitado
    void receiveMessages() {
        char buffer[512] = {0};
        while (true) {
            int bytes_read = read(sock, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                string received(buffer);
                string n(buffer);
                if (received.front() == '{' && received.back() == '}') {
                    cout << "Mensaje recibido JSON: " << received << endl;
                    json json_msg = StringToJSON(received);
                    string message_type = json_msg["type"];
                    if (message_type == "RESPONSE") {
                    string result = json_msg["result"];
                    if (result == "SUCCESS") {
                        cout << "Te has registrado correctamente, ahora puedes enviar mensajes." << endl;
                    } else if (result == "USER_ALREADY_EXISTS") {
                        cout << "El nombre de usuario ya está en uso." << endl;
                        break;
                    }
                } else if (message_type == "NEW_USER") {
                    string new_user = json_msg["username"];
                    cout << "Nuevo usuario conectado: " << new_user << endl; // Procesa el mensaje de nuevo usuario
                }

                } else {
                    cout <<"Mensaje no json: " << n;
                }
            } else if (bytes_read == 0) {
                close(sock);
                break;
            }
        }
    }

    void sendMessage(MessageType type, string message){
        json json_msg = makeJSON(type, message);
        string msg = JSONToString(json_msg);
        send(sock, msg.c_str(), msg.size(), 0);
        cout<<"Mensaje enviado json: " << msg << endl;
    }

    void sendIdentify(MessageType type, string message){
        json json_msg = makeIDENTIFY(type, message);
        string msg = JSONToString(json_msg);
        send(sock, msg.c_str(), msg.size(), 0);
        cout<<"Mensaje enviado json: " << msg << endl;
    }

public:
    string user_name;
    string status;
    Client(const string &ip, int port) : ip(ip), server_port(port) {}

    int connectToServer() {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(server_port);
        inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
            cout << "Server not connected" << endl;
            return -1;
        }
        thread receiveThread(&Client::receiveMessages, this);
        receiveThread.detach();
        return 0;
    }

    void connection(){
        cout << "Hello, please insert your username" << endl;
        getline(cin, user_name);
        sendIdentify(IDENTIFY, user_name);
        while (true) {
            string message;
            getline(cin, message);
            sendMessage(PUBLIC_TEXT_FROM, message);
        }
    }

};