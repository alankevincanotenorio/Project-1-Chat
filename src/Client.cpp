#include <iostream>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
using namespace std;

class Client {
private:
    int sock = 0;
    struct sockaddr_in serv_addr;
    string server_ip;
    int server_port;

   void receiveMessages() {
        string buffer;
        char buf[1];
        while (true) {
            int bytes_read = read(sock, buf, 1);
            if (bytes_read > 0) {
                if (buf[0] == '\n') {
                    cout << "Response: " << buffer << endl;
                    buffer.clear();
                } else {
                    buffer += buf[0];
                }
            }
        }
    }

public:
    Client(const string &ip, int port) : server_ip(ip), server_port(port) {}

    void connectToServer() {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(server_port);
        inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr);
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
            cout << "Server not connected" << endl;
            return;
        }
        // Start a thread to receive messages from the server
        thread receiveThread(&Client::receiveMessages, this);
        receiveThread.detach();

        cout << "Hello, please insert your username" << endl;
        while (true) {
            string message;
            getline(cin, message);
            if (message == "exit") break;
            send(sock, message.c_str(), message.size(), 0);
        }
    }
};
