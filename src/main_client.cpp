#include "Client.cpp"
#include <iostream>

using namespace std;

int main(int args, char* port_char[]) {
    if(args != 2) {
        cerr  << "Please only execute the bin archive and insert the 4 server port digits" << endl;
        return 0;
    }
    int port = stoi(port_char[1]);
    unique_ptr<Client> client = make_unique<Client>("127.0.0.1", port);
    client->connectToServer();
    return 0;
}