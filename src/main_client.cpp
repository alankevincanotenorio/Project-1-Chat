#include "cliente.cpp"
#include <iostream>
using namespace std;
int main() {
    unique_ptr<Client> client = make_unique<Client>("127.0.0.1", 8080);
    client->connectToServer();
    client->sendRequest();
    return 0;
}