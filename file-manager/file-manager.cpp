#include "file-manager.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

bool is_file_exist(string file_path)
{
    ifstream file(file_path.c_str());
    if (file.is_open())
    {
        file.close();
        return true;
    }
    else
        return false;
}

int get_file_size(string file_path)
{
    ifstream file(file_path.c_str());
    file.seekg(0, ios_base::end); //use seekg to set the cursor position to the end of the file 
    int size = file.tellg(); //use tllg() to get the size of the file
    file.close();
    return size;
}

void send_file(int socket, string file_path, int file_size)
{
    int total_bytes_sent = 0;
    char buffer[1024];
    ifstream file(file_path.c_str(), ios::binary);
    while (total_bytes_sent < file_size)
    {
        file.read(buffer, sizeof(buffer));
        // cout << buffer;
        int bytes_sent = send(socket, buffer, sizeof(buffer), 0);
        total_bytes_sent += bytes_sent;
        memset(buffer, 0, sizeof(buffer));
    } 
    file.close();
    
}

void receive_file(int socket, string file_path, int file_size)
{
    int total_bytes_received = 0;
    char buffer[1024];
    ofstream file(file_path, ios::binary);
    while (total_bytes_received < file_size)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(socket, buffer, sizeof(buffer), 0);
        // cout << buffer; 
        file << buffer;
        total_bytes_received += bytes_received;
    }
    file.close();

}
