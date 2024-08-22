#include <iostream>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

class Client {
private:
    int sock = 0;
    struct sockaddr_in serv_addr;
    string server_ip;
    int server_port;

public:
    Client(const string &ip, int port) : server_ip(ip), server_port(port) {}

    void connectToServer() {
        sock = socket(AF_INET, SOCK_STREAM, 0);

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(server_port);

        // Convertir dirección IP
        inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr);

        // Conectar al servidor
        connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        
    }

    void sendRequest() {
        string message = "Hola, servidor!";
        send(sock, message.c_str(), message.size(), 0);
        cout << "Mensaje enviado: " << message << endl;

        // Leer la respuesta
        char buffer[1024] = {0};
        read(sock, buffer, 1024);
        string response(buffer);
        cout << "Respuesta del servidor: " << response << endl;

        // // Leer entrada del usuario
        //     std::cout << "Ingresa un mensaje (escribe 'si' para salir): ";
        //     std::getline(std::cin, user_input);

        //     // Enviar mensaje al servidor
        //     send(sock, user_input.c_str(), user_input.size(), 0);
    }

    ~Client() {
        close(sock);
    }
};