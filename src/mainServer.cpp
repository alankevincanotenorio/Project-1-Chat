#include <iostream>
#include "Server.cpp"

int main(int argc, char* argv[]) {
    if(argc != 3) {
        cerr  << "Please only execute the bin archive and insert the server port" << endl;
        return 0;
    }
    string ip = argv[1];
    int port = stoi(argv[2]);
    unique_ptr<Server> server = make_unique<Server>(ip, port);
    server->initSocket();
    server->connectClient();
    return 0;
}