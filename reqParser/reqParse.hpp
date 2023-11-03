#pragma once

#include <cstddef>
#include <cstdlib>
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
#include "../Server.hpp"

#define NOT_DONE 0
#define DONE_WITH_HEADERS 1
#define DONE 2
#define CLOSE 3
#define BUFF_SIZE 1024

struct Client {
    Client(int fd, std::ifstream* i, std::ofstream* o);
	void	reset();

    std::map<std::string, Location>::iterator   location;
    unsigned long                               contentLength, maxBodySize;
    int                                         fd, statusCode;
	bool										isCgi;
    size_t                                      state, chunkSize, buffSize, position;
    char                                        buf[BUFF_SIZE + 1];
    std::string                                 method, target, version, host, sizeDept, headersBuf, cgiFileName, cgiScript;
    std::map<std::string, std::string>          headers;
    std::ifstream                               *is;
    std::ofstream                               *outfile;

};

void                                        moveBuf( Client & request, int amount );
void                                        bodyParser(Client & req);
void                                        reqParser(Client & request, int sock, std::vector<Server> &serv);
std::map<std::string, Location>::iterator   findServ(unsigned long &max_body_size, std::vector<Server>& serv, const std::string &host, const std::string &url);
