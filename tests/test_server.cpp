#include <gtest/gtest.h>
#include "../src/Server.cpp"
#include "../src/cliente.cpp"

//test para probar el constructor del Server
TEST(ServerTest, ConstructorInit){
    int expected_port = 8080;
    unique_ptr<Server> server = make_unique<Server>(expected_port);
    EXPECT_EQ(server->getPort(), expected_port);
}

//test para probar la creacion de socket
TEST(ServerTest, CreateSocket){
    int expected_port = 8080;
    unique_ptr<Server> server = make_unique<Server>(expected_port);
    server->init_socket();
    EXPECT_NE(server->getServer_fd(), -1);
}

//test para verificar si un usuario ha sido conectado al Server
TEST(ServerTest, UserConnected) {
    unique_ptr<Server> server = make_unique<Server>(8080);
    unique_ptr<cliente> client = make_unique<cliente>("127.0.0.1", 8080);    
    server->connect_server();
    client->connect_client();
    EXPECT_EQ(server->getConnectStatus(), true);
}

//test para verificar si un usuario no ha sido conectado al Server, en este caso, cuando no se registre
TEST(ServerTest, UserUnconnected) {
    unique_ptr<Server> server = make_unique<Server>(8080);
    unique_ptr<cliente> client = make_unique<cliente>("127.0.0.1", 8080);    
    server->connect_server();
    client->connect_client();
    EXPECT_NE(server->getConnectStatus(), true);
}

//test para verificar si un usuario ha sido registrado correctamente
TEST(ServerTest, UserRegister) {
    unique_ptr<Server> server = make_unique<Server>(8080);
    unique_ptr<cliente> client = make_unique<cliente>("127.0.0.1", 8080);    
    server->connect_server();
    client->connect_client();   
    EXPECT_EQ(server->getUsername(), "Juan");
}

//test para verificar si un usuario no esta registrado en el JSON
//pendiente

//test para el deconstructor pendiente