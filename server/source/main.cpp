#include "server.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{
    string ip_address = "127.0.0.1";
    int port;
    cout << "Enter port number: ";
    cin >> port;
    Server server(ip_address, port);
    server.work();
    server.~Server();
    return 0;
}
