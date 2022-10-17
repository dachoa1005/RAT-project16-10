#include "../../file-manager/file-manager.h"
#include "client.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>    // for types
#include <sys/socket.h>   // for socket
#include <sys/sendfile.h> // for sendfile()
#include <sys/uio.h>      // for uio
#include <netinet/in.h>   // for sockaddr_in
#include <errno.h>
#include <arpa/inet.h>
#include <fstream>

using namespace std;

Client::Client(string ip_address, int port)
{
    // create client socket
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip_address.c_str());
    server_address.sin_port = htons(port);

    // create socket to connect to server
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        cout << "Error creating socket" << endl;
        exit(1);
    }

    // connect to server
    int connect_status = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connect_status < -1)
    {
        cout << "Error connecting" << endl;
        exit(1);
    }
    cout << "Connected to server" << endl;
}

void Client::work()
{
    // get present working directory
    system("touch pwd.txt");
    system("pwd > pwd.txt");
    ifstream pwd_file("pwd.txt");
    string pwd;
    pwd_file >> pwd;
    pwd_file.close();
    string file_save_place = pwd + "/recv-files/";

    // cout << file_save_place << endl;

    int choice{0};
    char buffer[1024] = {0};
    string file_name{""};
    int file_size{0};
    string file_path{""};
    string message{""};
    int flag = 0; // flag to decide whether to send file or not: 1 = send file || 0 = re-enter file path || -1 = cancel send file.

    while (true)
    {
        cout << "--------------------------------" << endl;
        cout << "Waiting for command from server" << endl;
        read(client_socket, &choice, sizeof(choice));
        cout << "Command received: " << choice << endl;
        switch (choice)
        {
        case 1:
        {
            // receive file
            cout << "Receive file from server" << endl;
            recv(client_socket, buffer, sizeof(buffer),0); // receive "cancel" or file_name + " " + file_size
            if (strcmp(buffer, "cancel") == 0)
            {
                // cancel
                cout << "Server canceled" << endl;
                break;
            }
            else
            {
                // receive file name and file size
                message = buffer;
                // cout << buffer << endl;

                //process message to get file name and file size
                file_name = message.substr(0, message.find(" "));
                file_size = stoi(message.substr(message.find(" ") + 1));

                cout << "File name: " << file_name << " ,file size: " << file_size << " bytes."<< endl;
                file_path = file_save_place + file_name;
                // cout << file_path << endl;
                receive_file(client_socket, file_path, file_size);
                cout << "File received." << endl;
            }
            break;
        }
        case 2:
        {
            // send file to server
            cout << "Send file to server" << endl;
            flag = 0;
            while (flag == 0)
            {
                recv(client_socket, buffer, sizeof(buffer),0);
                if (strcmp(buffer, "cancel") == 0) 
                {
                    flag = -1;
                    send(client_socket, &flag, sizeof(flag), 0);
                    cout << "Cancel send file" << endl;
                    break;
                }

                file_path = string(buffer);
                if (is_file_exist(file_path) == false)
                {
                    flag = 0;
                    send(client_socket, &flag, sizeof(flag), 0); // send flag = 0 to server to re-enter file_path or cancel
                }
                else 
                {
                    flag = 1; // send flag =1 to server to continue sending file
                    send(client_socket, &flag, sizeof(flag), 0);
                    break;
                }
            }

            if (flag == 1) // send file
            {
                cout << "File path: " << file_path << endl;
                file_name = file_path.substr(file_path.find_last_of("/") + 1);
                file_size = get_file_size(file_path);
                cout << "File name: " << file_name << " ,File size: " << file_size << endl;
                message = file_name + " " + to_string(file_size);
                strcpy(buffer, message.c_str());
                send(client_socket, buffer, sizeof(buffer),0); //send file name and size
                send_file(client_socket, file_path.c_str(), file_size);
                cout << "File sent." << endl;
            }
            break;
        }

        case 3:
        {
            // kill process
            string process_name;
            cout << "Kill process" << endl;
            recv(client_socket, buffer, sizeof(buffer), 0);
            string command = "killall " + string(buffer);
            int n = system(command.c_str()); // 
            send(client_socket, &n, sizeof(n), 0); //return result to server
            break;
        }

        case 4:
        {
            // create process
            break;
        }
        case 0:
        {
            cout << "Server end the session." << endl;
            close(client_socket);
            exit(0);
        }
        default:
        {
            cout << "Invalid choice" << endl;
            break;
        }
        }
    }
}
