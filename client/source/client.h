#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

class Client
{
private:
    int client_socket;
    sockaddr_in server_address;
public:
    Client(std::string ip_address, int port);
    void work();
    // connect
    // receive message
    // process message
    // send message
    // close connection
};

#endif