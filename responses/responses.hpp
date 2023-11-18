#pragma once

#include <iostream>
#include <unistd.h>
#include <map>
#include <vector>
#include <string>
#include "../Client.hpp"
#include "../FileType.hpp"
#include <sys/stat.h>
#include <dirent.h>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstring>
#include <csignal>




typedef struct Client Client;
void responses(Client &client);
std::string cur_time();
int s_header(Client& client, int client_socket, std::string status, std::string type);
void c_base(std::string& str, int n, const int &base);
void s_chank(Client& client, int fd, const char *content, const int size);
int is_dir(std::string& str);
void get_target(Client &client, std::string &get_query);
std::string get_ex(std::string str);
void get(Client &client);
int is_cgi(Client &client);
int get_index(Client &client);
int auto_index(Client &client);
void cgi(Client &client);
void redirect(Client &client, std::string target);
void ft_delete(Client &client);
int error_handling(Client &client);
int check_header(std::string header);
std::string get_page(Client &client, int n);
