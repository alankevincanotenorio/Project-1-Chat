#include <iostream>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include "../libs/json.hpp"
using json = nlohmann::json;

using namespace std;

class servidor {
private:
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int port;
    bool socket_open;

public:
    servidor(int port) : port(port) {}

    //metodo que crea e inicializa el socket
    void init_socket() {
        if (socket_open) {
            cerr << "El socket principal ya está abierto" << endl;
            return;
        }

        // Crear el socket
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            cerr << "Error al crear el socket: " << strerror(errno) << endl;
            return;
        }

        // Configurar opciones del socket
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
            cerr << "Error al configurar el socket: " << strerror(errno) << endl;
            close(server_fd);
            return;
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        // Bind del socket
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            cerr << "Error al usar bind: " << strerror(errno) << endl;
            close(server_fd);
            return;
        }

        // Escuchar conexiones
        if (listen(server_fd, SOMAXCONN) < 0) {
            cerr << "Error en listen: " << strerror(errno) << endl;
            close(server_fd);
            return;
        }

        socket_open = true;
    }

    //metodo destructor del objeto
    ~servidor() {
        if (socket_open) {
            cout << "Servidor destruido" << endl;
            if (close(server_fd) == -1) {
                cerr << "Error al cerrar el socket: " << strerror(errno) << endl;
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
};
