#include "client.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>

using namespace std;

int main(int argc, char const *argv[])
{
    string ip_address = "127.0.0.1";
    int port;
    cout << "Enter port number: ";
    cin >> port;
    Client client(ip_address, port);
    client.work();
    client.~Client();
    return 0;
}
