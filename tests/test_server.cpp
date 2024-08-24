#include <gtest/gtest.h>
#include "../src/Server.cpp"
#include "../src/Client.cpp"

//test to test the server constructor
TEST(ServerTest, ConstructorInit){
    int expected_port = 8080;
    unique_ptr<Server> server = make_unique<Server>(expected_port);
    EXPECT_EQ(server->getPort(), expected_port);
}

//test to test the creation of the server
TEST(ServerTest, CreateSocket){
    int expected_port = 8080;
    unique_ptr<Server> server = make_unique<Server>(expected_port);
    server->init_socket();
    EXPECT_NE(server->getServer_fd(), -1);
}

//test to check if a user has been connected to the Server
TEST(ServerTest, UserConnected) {
    unique_ptr<Server> server = make_unique<Server>(8080);
    unique_ptr<Client> client = make_unique<Client>("127.0.0.1", 8080);    
    server->connect_server();
    client->connect_client();
    EXPECT_EQ(server->getConnectStatus(), true);
}

//test to check if a user has not been connected to the Server, in this case, when not registered
TEST(ServerTest, UserUnconnected) {
    unique_ptr<Server> server = make_unique<Server>(8080);
    unique_ptr<Client> client = make_unique<Client>("127.0.0.1", 8080);    
    server->connect_server();
    client->connect_client();
    EXPECT_NE(server->getConnectStatus(), true);
}

//test to check if a user has been registered correctly
TEST(ServerTest, UserRegister) {
    unique_ptr<Server> server = make_unique<Server>(8080);
    unique_ptr<Client> client = make_unique<Client>("127.0.0.1", 8080);    
    server->connect_server();
    client->connect_client();   
    EXPECT_EQ(server->getUsername(), "Juan");
}

//test para verificar si un usuario no esta registrado en el JSON
//pendiente

//test para el deconstructor pendiente