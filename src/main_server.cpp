#include <iostream>
#include "Server.cpp"

int main() {
    unique_ptr<Server> server = make_unique<Server>(8080);
    server->init_socket();
    return 0;
}