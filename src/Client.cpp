#include <iostream>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include "Protocol.cpp"
using namespace std;

class Client {
private:
    int sock = 0;
    struct sockaddr_in serv_addr;
    string ip;
    int server_port;
    unique_ptr <thread> receiveThread;

    void receiveMessages() {
        char buf[512];
        while (true) {
            int bytes_read = read(sock, buf, sizeof(buf) - 1);
            if (bytes_read > 0) {
                buf[bytes_read] = '\0';
                string message(buf);
                cout << message;
            }
        }
    }


public:
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

    //falta que se impriman bien los mensajes pq se imprimen los mismo json jssj 
    void connection(){
        cout << "Hello, please insert your username" << endl;
        string messa;
        getline(cin, messa);
        identify(messa);
        while (true) {
            string message;
            getline(cin, message);
            sendMessage(message);
        }
    }

    //metodo para mandar la identificacion
    void identify(string message){
        json j = makeJSON(IDENTIFY, message);
        string m = JSONToString(j);
        send(sock, m.c_str(), m.size(), 0);
        //si recibe el protocolo negativo lo bota con return;
    }

    void sendMessage(string message){
        json j = makeJSON(PUBLIC_TEXT_FROM, message);
        string m = JSONToString(j);
        send(sock, m.c_str(), m.size(), 0);
        //si recibe el protocolo negativo lo bota con return;
    }
    //metodo para mandar mensaje general
    //metodo para mandar mensaje privado
    //metodo para mandar 

};