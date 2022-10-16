#include "server.h"
#include "../../file-manager/file-manager.h"
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

Server::Server(string ip_address, int port)
{
    // create server socket
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip_address.c_str());
    server_address.sin_port = htons(port);
    // create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        cout << "Error creating socket" << endl;
        exit(1);
    }

    // bind socket
    int bind_status = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (bind_status < -1)
    {
        cout << "Error binding socket" << endl;
        exit(1);
    }

    // listen
    int listen_status = listen(server_socket, 5); // listen for max 5 connections
    if (listen_status < -1)
    {
        cout << "Error listening" << endl;
        exit(1);
    }

    cout << "Server started, waiting for client to connect" << endl;

    // create new socket to connect to client
    sockaddr_in new_socket;
    socklen_t new_socket_size = sizeof(new_socket);
    new_socket_fd = accept(server_socket, (struct sockaddr *)&new_socket, &new_socket_size);
    if (new_socket_fd < -1)
    {
        cout << "Error accepting connection" << endl;
        exit(1);
    }

    cout << "Connected to client" << endl;
}

void Server::work()
{
    system("touch pwd.txt");
    system("pwd > pwd.txt"); // get current directory and save it to pwd.txt
    ifstream pwd_file("pwd.txt");
    string pwd;
    pwd_file >> pwd; // read pwd.txt and save it to pwd
    pwd_file.close();
    string file_save_place = pwd + "/recv-files/";

    cout << file_save_place << endl;

    int choice{0};
    string file_name{""};
    int file_size{0};
    string file_path{""};
    string message{""};
    char buffer[1024] = {0};
    char temp_buffer[1024] = {0};
    int flag{0}; // flag to decide whether to send file or not: 1 = send file || 0 = re-enter file path || -1 = cancel send file.

    while (true)
    {
        cout << "--------------------------------" << endl;
        cout << "1. Send file to client" << endl;
        cout << "2. Get file from client" << endl;
        cout << "3. Kill process" << endl;
        cout << "4. Delete registry" << endl;
        cout << "0. Exit" << endl;

        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();
        send(new_socket_fd, &choice, sizeof(choice), 0);
        // cout << "byte_write: " << byte_write << endl;
        switch (choice)
        {
        case 1: // send file to client
        {
            file_path = "";
            flag = 0;
            cout << "Send file to client" << endl;
            while (flag == 0)
            {
                cout << "Enter file path: ";
                getline(cin, file_path);
                if (file_path == "cancel")
                {
                    flag = -1;
                    cout << "Cancel sending file" << endl;
                    strcpy(temp_buffer, "cancel");
                    send(new_socket_fd, temp_buffer, sizeof(temp_buffer), 0); //send "cancel" to client
                    break;
                }
                if (is_file_exist(file_path) == false)
                {
                    flag = 0;
                    cout << "File not found! Enter cancel to break" << endl;
                }
                else
                {
                    flag = 1;
                }
            }

            // get file name

            if (flag == 1)
            {
                file_name = file_path.substr(file_path.find_last_of("/") + 1); // get file name from input file path
                file_size = get_file_size(file_path);
                message = file_name + " " + to_string(file_size); // send header message include file name and file size
                send(new_socket_fd, message.c_str(), message.length(), 0);
                cout << "File name: " << file_name << " ,file size: " << file_size << " bytes" << endl;
                send_file(new_socket_fd, file_path, file_size);
                cout << "File sent." << endl;
            }
            break;
        }
        case 2: // recieve file from client
        {
            file_path = "";
            cout << "Recieved file from client" << endl;
            flag = 0;
            while (flag == 0)
            {
                cout << "Enter file path:";
                getline(cin, file_path);

                if (file_path == "cancel")
                {
                    // cancel receive
                    flag = -1;
                    cout << "Cancel received" << endl;
                    strcpy(temp_buffer, "cancel");
                    send(new_socket_fd, temp_buffer, sizeof(temp_buffer), 0);
                    break;
                }

                strcpy(temp_buffer, file_path.c_str());
                send(new_socket_fd, temp_buffer, sizeof(temp_buffer), 0); // send file path to client 
                recv(new_socket_fd, &flag, sizeof(flag), 0);
                if (flag == 0) // re-enter file path or cancel
                {
                    cout << "File not found! Enter cancel to break" << endl;
                }
                else if (flag == 1) //continue receiving file
                {
                    cout << "File found!" << endl;
                    break;
                }
            }

            if (flag == 1) // recieve file
            {
                recv(new_socket_fd, buffer, sizeof(buffer), 0); //recv file name and size
                message = buffer;

                //process message to get file name and size
                file_name = message.substr(0, message.find(" "));
                file_size = stoi(message.substr(message.find(" ") + 1));
                cout << "File name: " << file_name << " ,Flie size: " << file_size << " bytes." << endl;
                file_path = file_save_place + file_name;
                receive_file(new_socket_fd, file_path, file_size);
                cout << "File received." << endl;
            }
            break;
        }

        case 3:
        {
            string process_name;
            //kil process
            cout << "Enter process name: ";
            cin >> process_name;
            send(new_socket_fd, process_name.c_str(), sizeof(process_name), 0);
            break;
        }
        
        case 4:
        {
            //delete registry
            break;
        }
        case 0:
        {
            cout << "Server end the session." << endl;
            close(new_socket_fd);
            close(server_socket);
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

Server::~Server()
{
    close(server_socket);
    close(new_socket_fd);
}
