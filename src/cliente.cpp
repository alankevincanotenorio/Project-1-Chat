#include <iostream>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

class cliente{
private:
    int sock = 0;
    struct sockaddr_in serv_addr;
    string server_ip;
    int server_port;


public:
    cliente(const string &ip, int port) : server_ip(ip), server_port(port) {}
    void connect_client(){
        
    }
};