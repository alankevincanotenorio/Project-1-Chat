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

    void receiveMessages() {
        char buffer[512];
        while (true) {
            int bytes_read = read(sock, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                string message(buffer);
                cout << message;
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
    }

    void sendIdentify(MessageType type, string message){
        json json_msg = makeIDENTIFY(type, message);
        string msg = JSONToString(json_msg);
        send(sock, msg.c_str(), msg.size(), 0);
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