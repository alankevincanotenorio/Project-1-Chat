#include <iostream>
#include "servidor.cpp"

int main() {
    unique_ptr<servidor> server = make_unique<servidor>(8080);
    server->init_socket();
    return 0;
}
