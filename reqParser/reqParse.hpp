#pragma once

#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <dirent.h>
#include "../Server.hpp"
#include <ctime>

#define NOT_DONE 0
#define DONE_WITH_HEADERS 1
#define DONE 2
#define CLOSE 3
#define BUFF_SIZE 1024

struct Client {
    Client(int fd, std::ifstream* i, std::ofstream* o);
	void	reset();
    unsigned long                               contentLength, maxBodySize;
    int                                         fd, statusCode;
    size_t                                      state, chunkSize, buffSize, position;
    char                                        buf[BUFF_SIZE + 1];
    std::string                                 method, target, version, host, sizeDept, headersBuf, response;
    std::map<std::string, std::string>          headers;
    std::ifstream                               *is;
    std::ofstream                               *outfile;
    int opened, is_cgi, pid;
    DIR* dir;
    std::time_t currentTime;
    std::string state_string;
    std::map<std::string, Location>::iterator   location;
	bool										isCgi, isDir;
	std::string									cgiFileName, cgiScript;
	std::string 								path, query, fullPath;
    std::time_t request_time; 
};

void                                        moveBuf( Client & request, int amount );
void                                        bodyParser(Client & req);
void                                        reqParser(Client & request, int sock, std::vector<Server> &serv);
std::map<std::string, Location>::iterator   findServ(unsigned long &max_body_size, std::vector<Server>& serv, const std::string &host, const std::string &url);
void										postHandler(Client & request);
