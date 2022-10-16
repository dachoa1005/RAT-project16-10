#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>


class Server
{
private:
    int server_socket;
    int new_socket_fd; // new socket to connect to client
    sockaddr_in server_address;
public:
    Server(std::string ip_address, int port);
    void work();
    //send message
    //receive message
    //close connection
    ~Server();
};

#endif 