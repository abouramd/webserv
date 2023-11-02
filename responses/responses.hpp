#pragma once

#include <iostream>
#include <unistd.h>
#include <map>
#include <vector>
#include <string>
#include "../Client.hpp"
#include "../FileType.hpp"
#include <sys/stat.h>

typedef struct Client Client;
void responses(Client &client);
std::string cur_time();
void s_header(int client_socket, std::string status, std::string type);
void c_base(std::string& str, int n, const int &base);
void s_chank(int fd, const char *content, const int size);
int is_dir(std::string& str);
void get_target(Client &client, std::string &get_query);
std::string get_ex(std::string str);
void get(Client &client);