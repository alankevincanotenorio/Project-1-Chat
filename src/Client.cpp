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

    //chage to switch case
    void handleMessageType(const json& json_msg) {
        string message_type = json_msg["type"];
        if (message_type == "RESPONSE") {
            string result = json_msg["result"];
            string operation = json_msg["operation"];
            if (operation == "NEW_ROOM") {
            if (result == "SUCCESS") {
                cout << "El cuarto '" << json_msg["extra"] << "' fue creado exitosamente." << endl;
            } else if (result == "ROOM_ALREADY_EXISTS") {
                cout << "El cuarto '" << json_msg["extra"] << "' ya existe." << endl;
            }
        }
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
                cout << "Sigue las reglas porfa" << endl;
                close(sock);
                exit(0);
            }  else if (result == "INVALID") {
                cout << "Mensaje invalido" << endl;
                close(sock);
                exit(0);
            } else if (result == "NO_SUCH_USER") {
                cout << "El usuario " << json_msg["extra"] << " no existe." << endl;
            }
        } else if (message_type == "NEW_USER") {
            string user_name = json_msg["username"];
            cout << "Nuevo usuario conectado: " << status << user_name << endl; 

        } else if (message_type == "NEW_STATUS") {
            string s = json_msg["status"];
            string user_name = json_msg["username"];
            updateUserStatusMap(user_name, s);
            if (user_name != this->user_name) {
                cout << "Nuevo estado actualizado: " << user_status_map[user_name] << user_name << endl;
            } else {
                setStatus(s);
                cout << "Tu estado ha sido actualizado a: " << status << endl;
            }
        }  else if (message_type == "USER_LIST") {
            json users_json = json_msg["users"];
            cout << "Lista de usuarios en la sala:" << endl;
            for (auto& [username, status] : users_json.items()) {
                cout << username << ": " << status << endl;
            }
        } else if (message_type == "TEXT_FROM") {
            string text = json_msg["text"];
            string from_user = json_msg["username"];
            cout << from_user << " (mensaje privado): " << text << endl;
        } else if(message_type == "PUBLIC_TEXT_FROM"){
            string text = json_msg["text"];
            string n = json_msg["username"];
            string st = user_status_map[n];
            if(st.empty()) st = "\U0001F600";
            cout << st << n << ": " << text << endl;
        }  else if(message_type == "DISCONNECTED"){
            string n = json_msg["username"];
            cout << n << " has been disconnected" << endl;
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
                    json json_msg = StringToJSON(received);
                    handleMessageType(json_msg);
                }
                // } else {
                //     cout << "Mensaje no JSON recibido: " << received;
                // }
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
                json_msg = makePbtext(type, message);
                break;
            case DISCONNECT:
                json_msg = makeDISCONNECT(type);
                break;
            case TEXT:
                json_msg = makeTEXT(type, message, target_user);
                break;
            case NEW_ROOM: 
                json_msg["type"] = "NEW_ROOM";
                json_msg["roomname"] = message;
                break;
            default:
                //lo mejor tambien podria ser que mandemos el mensaje sin hacerlo json y ya en el server verificar si es un json o no
                json_msg["message"] = message; //hacemos un  json sin tipo para manejar mensajes sin ningun comando
                break;
        }
        string msg = JSONToString(json_msg);
        send(sock, msg.c_str(), msg.size(), 0);
        if (type == PUBLIC_TEXT) {
            cout << status << " " << user_name << ": " << message << endl;
        }
        cout << "Mensaje enviado json: " << msg << endl;
    }

    //maybe tener una enum
    void checkCommand(const string& input, string username = "") {
        //id
        if (input.substr(0, 3) == "id ") {
            string user_name = input.substr(3);
            
            sendMessage(IDENTIFY, user_name);
        } else if(!is_identified){
            cout << "no te identificaste correctamente" << endl;
            exit(0);
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
            else if (input.substr(0, 4) == "new ") {
                string roomname = input.substr(4);
                sendMessage(NEW_ROOM, roomname);  // Envía el comando NEW_ROOM
            }

            else{
                sendMessage(NONE,input); //esto esta muy raro aca se debe manejar cuando no ingresas  ningun comando
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