#include <iostream>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include "../libs/json.hpp"
using json = nlohmann::json;
using namespace std;

class Server {
private:
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int port;
    bool socket_open;

public:
    Server(int port) : port(port) {}

        //method that creates and initializes the socket    
        void init_socket() {
        if (socket_open) {
            cerr << "The main socket is open" << endl;
            return;
        }
        // Create socket
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            cerr << "Error creating socket: " << strerror(errno) << endl;
            return;
        }
        // Configurar opciones del socket
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
            cerr << "Error configuring socket: " << strerror(errno) << endl;
            close(server_fd);
            return;
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        // Bind del socket
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            cerr << "Error using bind: " << strerror(errno) << endl;
            close(server_fd);
            return;
        }
        // Escuchar conexiones
        if (listen(server_fd, SOMAXCONN) < 0) {
            cerr << "Error listen: " << strerror(errno) << endl;
            close(server_fd);
            return;
        }
        socket_open = true;
    }

    //not implemented 
    void connect_server(){
        
    }

    //destructive method
    ~Server() {
        if (socket_open) {
            cout << "Server destroyed" << endl;
            if (close(server_fd) == -1) {
                cerr << "Error closing socket: " << strerror(errno) << endl;
            }
        }
    }

    // GETTERS
    int getPort() const {
        return port;
    }

    int getServer_fd() const {
        return server_fd;
    }

    bool getSocket_open() const {
        return socket_open;
    }

    //not implemented
    string getUsername() {
        return "User";
    }

};