#include <iostream>
#include "Server.cpp"

int main() {
    unique_ptr<Server> server = make_unique<Server>(8080);
    server->initSocket();
    server->connectClient();
    return 0;
}