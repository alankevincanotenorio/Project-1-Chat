#include "Client.cpp"
#include <iostream>

using namespace std;

int main(int args, char*argv[]) {
    if(args != 3) {
        cerr  << "Please only execute the bin archive and insert the 4 server port digits" << endl;
        return 0;
    }
    string server_ip = argv[1];
    int port = stoi(argv[2]);
    unique_ptr<Client> client = make_unique<Client>(server_ip, port);
    if(client->connectToServer() == -1) return 0;
    client->connection();
    return 0;
}