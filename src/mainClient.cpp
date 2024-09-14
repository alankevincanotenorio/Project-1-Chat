#include "Client.cpp"
#include <iostream>

using namespace std;

/**
 * @brief The entry point of the program. Initializes a client.
 * 
 * @param args The number of command-line arguments. Should be 3 (bin name, server IP, and port).
 * @param argv The array of command-line arguments:
 *             - argv[1]: Server IP address (string).
 *             - argv[2]: Server port.
 * 
 * @return int Returns 0 when the program terminates.
 * 
 */
int main(int args, char*argv[]) {
    if(args != 3) {
        cerr  << "Please only execute the bin archive, insert the server ip and the server port digits" << endl;
        return 0;
    }
    string server_ip = argv[1];
    int port = stoi(argv[2]);
    unique_ptr<Client> client = make_unique<Client>(server_ip, port);
    if(client->connectToServer() == -1) return 0;
    client->connection();
    return 0;
}