#include <iostream>
#include "Server.cpp"

/**
 * @brief The entry point of the server program.
 * 
 * @param argc The number of command-line arguments. Should be 2 (program name and server port).
 * @param argv The array of command-line arguments, in this case, argv[1]: Server port (integer).
 * 
 * @return int Returns 0 when the program terminates.
 * 
 */
int main(int argc, char* argv[]) {
    if(argc != 2) {
        cerr  << "Please only execute the bin archive and insert the server port" << endl;
        return 0;
    }
    int port = stoi(argv[1]);
    unique_ptr<Server> server = make_unique<Server>("localhost", port);
    server->initSocket();
    server->connectClient();
    return 0;
}