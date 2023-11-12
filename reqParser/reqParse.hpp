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
#include "String.hpp"

#define NOT_DONE 0
#define DONE_WITH_HEADERS 1
#define DONE 2
#define CLOSE 3
#define BUFF_SIZE 1024

#define BOUND 43
#define HEAD 44
#define BOD 45
#define CRLF 46
#define NON 404

struct Client {
    Client(int fd, std::ifstream* i, std::ofstream* o, std::map<int, std::string> e, std::map<int, std::string> ed);

    std::map<int, std::string> error_page;
    std::map<int, std::string> error_page_dfl;
	void	reset();
    unsigned long                               contentLength, maxBodySize;
    int                                         fd, statusCode;
    size_t                                      lastState, boundState, state, chunkSize, buffSize, position;
    String                                      boundBuf, contentType;
    char                                        buf[BUFF_SIZE + 1];
    std::string                                 method, target, version, host, sizeDept, headersBuf, boundary;
    std::map<std::string, std::string>          headers;
    std::ifstream                               *is;
    std::ofstream                               *outfile;
    int opened, is_cgi, pid;
    DIR* dir;
    std::time_t currentTime;
    std::string state_string;
    std::pair<std::string, Location>            location;
	bool										isCgi, isDir, isBound, beenThere;
	std::string									cgiFileName, cgiScript;
	std::string 								path, query, fullPath;
    std::time_t request_time; 
};

void                                        moveBuf( Client & request, int amount );
void                                        bodyParser(Client & req);
void                                        reqParser(Client & request, int sock, std::vector<Server> &serv);
void                                        unBound(Client & request);

void  findServ(Client &client, std::vector<Server>& serv, const std::string &host, const std::string &url);

void										postHandler(Client & request);
