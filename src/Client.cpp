#include <iostream>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

class Client{
private:
    int sock = 0;
    struct sockaddr_in serv_addr;
    string server_ip;
    int server_port;

public:
    Client(const string &ip, int port) : server_ip(ip), server_port(port) {}

    void connectToServer(){
        sock = socket(AF_INET, SOCK_STREAM, 0);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(server_port);
        inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr);
        if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
            cout<<"server not connected"<<endl;
            return;
        }
        cout << "Hello, please insert a message to the server" << endl;
        string user;
        getline(cin, user);
        send(sock, user.c_str(), user.size(), 0);
        char buffer[1024] = {0};
        read(sock, buffer, 1024);
        cout << "Server response: " << buffer << endl;
    }

};