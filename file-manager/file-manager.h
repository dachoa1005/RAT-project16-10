#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <iostream>
#include <string>

bool is_file_exist(std::string file_path);
int get_file_size(std::string file_path);
void send_file(int socket, std::string file_path, int file_size);
void receive_file(int socket, std::string file_path, int file_size);

#endif // 