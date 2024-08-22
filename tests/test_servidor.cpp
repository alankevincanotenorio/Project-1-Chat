#include <gtest/gtest.h>
#include "../src/servidor.cpp"
#include "../src/cliente.cpp"

//test para probar el constructor del servidor
TEST(ServidorTest, ConstructorInit){
    int expected_port = 8080;
    unique_ptr<servidor> server = make_unique<servidor>(expected_port);
    EXPECT_EQ(server->getPort(), expected_port);
}

//test para probar la creacion de socket
TEST(ServidorTest, CreateSocket){
    int expected_port = 8080;
    unique_ptr<servidor> server = make_unique<servidor>(expected_port);
    server->init_socket();
    EXPECT_NE(server->getServer_fd(), -1);
}


//test para el deconstructor pendiente
TEST(ServidorTest, UserRegister) {
    unique_ptr<servidor> server = make_unique<servidor>(8080);
    unique_ptr<cliente> client = make_unique<cliente>("127.0.0.1", 8080);
    
    server->connect_server();
    client->connect_client();
    
    // Supongamos que "Juan" es el nombre de usuario que esperas después de la conexión
    EXPECT_EQ(server->getUsername(), "Juan");
}
