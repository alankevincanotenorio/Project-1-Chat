#include <iostream>
#include "Server.cpp"

int main(int argc, char* argv[]) {
    if(argc != 2) {
        cerr  << "Please only execute the bin archive and insert the server port" << endl;
        return 0;
    }
    int port = stoi(argv[1]);
    unique_ptr<Server> server = make_unique<Server>(port);
    server->initSocket();
    server->connectClient();
    return 0;
}