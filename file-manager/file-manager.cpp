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
    int size{0};
    ifstream file(file_path.c_str());
    file.seekg(0, ios_base::end); // use seekg to set the cursor position to the end of the file
    size = file.tellg();          // use tllg() to get the size of the file
    file.close();
    return size;
}

void send_file(int socket, string file_path, int file_size)
{
    int total_bytes_sent = 0;
    char buffer[1024];
    int bytes_sent = 0;
    ifstream file(file_path.c_str());
    while (total_bytes_sent < file_size)
    {
        memset(buffer, 0, sizeof(buffer));
        // cout << buffer;
        total_bytes_sent += bytes_sent;

        // send last buffer if file size is not multiple of 1024
        if (file_size - total_bytes_sent < 1024)
        {
            file.read(buffer, file_size - total_bytes_sent);
            // cout << buffer;
            bytes_sent = send(socket, buffer, file_size - total_bytes_sent, 0);
            total_bytes_sent += bytes_sent;
        }
        else 
        {
            file.read(buffer, sizeof(buffer));   
            bytes_sent = send(socket, buffer, sizeof(buffer), 0);
        }
        // cout << total_bytes_sent << endl;
    }
    file.close();
}

void receive_file(int socket, string file_path, int file_size)
{
    int total_bytes_received = 0;
    int bytes_received = 0;
    char buffer[1024];
    ofstream file{file_path};
    while (total_bytes_received < file_size)
    {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1)
        {
            cout << "Error receiving file" << endl;
            return;
        }
        total_bytes_received += bytes_received;
        file.write(buffer, bytes_received);        
        // cout << bytes_received << endl;
    }
    file.close();
}