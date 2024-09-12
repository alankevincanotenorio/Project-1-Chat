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

    unordered_map<string, string> user_status_map; //guarda los nuevos estados para que sea posible imprimirlos

    bool is_identified = false;

    // Función para manejar los mensajes según su tipo
    void handleMessageType(const json& json_msg) {
        string message_type_str = json_msg["type"];
        MessageType message_type = stringToMessageType(message_type_str);
        switch (message_type) {
            case RESPONSE: {
                string result = json_msg["result"];
                if (result == "SUCCESS") {
                    is_identified = true;
                    user_name = json_msg["extra"];
                    setStatus("ACTIVE");
                    cout << "Te has registrado correctamente, ahora puedes enviar mensajes." << endl;
                } else if (result == "USER_ALREADY_EXISTS") {
                    cout << "El nombre de usuario ya está en uso." << endl;
                    close(sock);
                    exit(0);
                } else if (result == "NOT_IDENTIFIED") {
                    cout << "Sigue las reglas por favor." << endl;
                    close(sock);
                    exit(0);
                } else if (result == "INVALID") {
                    cout << "Mensaje inválido." << endl;
                    close(sock);
                    exit(0);
                } else if (result == "NO_SUCH_USER") {
                    cout << "El usuario " << json_msg["extra"] << " no existe." << endl;
                }
                break;
            }
            case NEW_USER: {
                string user_name = json_msg["username"];
                cout << "Nuevo usuario conectado: " << user_name << endl;
                break;
            }
            case NEW_STATUS: {
                string new_status = json_msg["status"];
                string user_name = json_msg["username"];
                updateUserStatusMap(user_name, new_status);
                if (user_name != this->user_name) {
                    cout << "Nuevo estado actualizado: " << user_status_map[user_name] << " para " << user_name << endl;
                } else {
                    setStatus(new_status);
                    cout << "Tu estado ha sido actualizado a: " << status << endl;
                }
                break;
            }
            case USER_LIST: {
                json users_json = json_msg["users"];
                cout << "Lista de usuarios en la sala:" << endl;
                for (auto& [username, status] : users_json.items()) {
                    cout << username << ": " << status << endl;
                }
                break;
            }
            case TEXT_FROM: {
                string text = json_msg["text"];
                string from_user = json_msg["username"];
                cout << from_user << " (mensaje privado): " << text << endl;
                break;
            }
            case PUBLIC_TEXT_FROM: {
                string text = json_msg["text"];
                string n = json_msg["username"];
                string st = user_status_map[n];
                if (st.empty()) st = "\U0001F600";  // Emoji de sonrisa si no hay estado
                cout << st << n << ": " << text << endl;
                break;
            }
            case DISCONNECTED: {
                string n = json_msg["username"];
                cout << n << " ha sido desconectado." << endl;
                break;
            }
            default:
                cout << "Tipo de mensaje no reconocido: " << message_type_str << endl;
                break;
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
                    json json_msg = json::parse(received);
                    handleMessageType(json_msg);
                }
            } else if (bytes_read == 0) {
                close(sock);
                break;
            }
        }
    }

    void sendMessage(MessageType type, const string& message, const string& target_user = "") {
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
                json_msg = makePublictxt(type, message);
                break;
            case DISCONNECT:
                json_msg = makeDISCONNECT(type);
                break;
            case TEXT:
                json_msg = makeTEXT(type, message, target_user);
                break;
            default:
                break;
        }
        string msg = json_msg.dump();
        if(type != NONE) {
            send(sock, msg.c_str(), msg.size(), 0);
            if (type == PUBLIC_TEXT) {
                cout << status << " " << user_name << ": " << message << endl;
            }
            cout << "Mensaje enviado json: " << msg << endl;
        } else{
            send(sock, message.c_str(), message.size(), 0);
            cout << "Mensaje no json: " << message << endl;
        } 
    }

    //maybe tener una enum
    void checkCommand(const string& input, string username = "") {
        //id
        if (input.substr(0, 3) == "id ") {
            string user_name = input.substr(3);
            
            sendMessage(IDENTIFY, user_name);
        } else{
            //status
            if (input.substr(0, 4) == "sts ") {
                string new_status = input.substr(4);
                sendMessage(STATUS, new_status);
                
            }
            //users list
            else if(input.substr(0, 6) == "users"){
                sendMessage(USERS, "");
            }
            //disconnect
            else if (input == "exit") {
                sendMessage(DISCONNECT, "");
                close(sock);
                cout << "Te has desconectado del servidor." << endl;
                exit(0);
            }
            //public message
            else if(input.substr(0, 3) == "pb "){
                string message = input.substr(3);            
                sendMessage(PUBLIC_TEXT, message);
            } 
            // private text
            else if (input.substr(0, 4) == "txt ") {
                size_t spacePos = input.find(" ", 4);
                if (spacePos != string::npos) {
                    string target_user = input.substr(4, spacePos - 4);
                    string private_message = input.substr(spacePos + 1);
                    sendMessage(TEXT, private_message, target_user);
                } else {
                    cout << "Formato incorrecto de mensaje, usa: txt <username> <mensaje>" << endl;
                }
            }
            else{
                sendMessage(NONE,input);
            }
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

    void setStatus(string stats){
        if (stats == "ACTIVE") status = "\U0001F600";
        else if (stats == "BUSY") status = "\U0001F623";
        else if (stats == "AWAY") status = "\U0001F914";
    }

    void updateUserStatusMap(const string& username, const string& new_status) {
        if (new_status == "ACTIVE") {
            user_status_map[username] = "\U0001F600";  // 😀
        } else if (new_status == "BUSY") {
            user_status_map[username] = "\U0001F623";  // 😥
        } else if (new_status == "AWAY") {
            user_status_map[username] = "\U0001F914";  // 🤔
        }
    }



};