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
#include "Server.hpp"
#include <ctime>
#include "String.hpp"

#define NOT_DONE 0
#define DONE_WITH_HEADERS 1
#define DONE 2
#define CLOSE 3
#define BUFF_SIZE 1024

#define AT_START 42
#define BOUND 43
#define HEAD 44
#define BOD 45

enum parseState {
    SPACE,
    CRLF,
    METHOD,
    TARGET,
    VERSION,
    HEADER,
    CHECK_ERROR,
    BODY
};

enum chunkState {
    SIZE,
    CRLF_CH,
    CHUNK,
};

struct Client {
    Client(int fd, std::ifstream* i, std::ofstream* o, std::map<int, std::string> e, std::map<int, std::string> ed);
    void    setEnv();

    std::map<int, std::string> error_page;
    std::map<int, std::string> error_page_dfl;
    std::string server_host, server_port, client_host, client_port;
    unsigned long                               contentLength, maxBodySize;
    int                                         fd, statusCode;
    parseState                                  pState, pNext;
    chunkState                                  chState, chNext;
    size_t                                      boundState, state, chunkSizeNum, buffSize, position;
    std::vector<char>                           boundBuf;
    char                                        buf[BUFF_SIZE + 1];
    std::string                                 contentType, chunkSizeStr, header, crlf, method, target, version, host, boundary, lastTwo;
    std::map<std::string, std::string>          headers, env;
    std::ifstream                               *is;
    std::ofstream                               *outfile;
    int opened, is_cgi, pid, checked, pro_state, write_f;
    DIR* dir;
    std::time_t currentTime;
    std::string state_string;
    std::pair<std::string, Location>            location;
	bool										atStart, isCgi, isDir, isBound, beenThere;
	std::string									uploadFile, cgiFileName, cgiInFileName, cgiScript;
	std::string 								path, query, fullPath;
    std::time_t request_time; 
  std::string tmp_error, chank;
};

void                                        moveBuf( Client & request, int amount );
void                                        bodyParser(Client & req);
void                                        reqParser(Client & request, int sock, std::vector<Server> &serv);
void                                        unBound(Client & request);
void                                        handleChunked(Client & request);

void                                        findServ(Client &client, std::vector<Server>& serv, const std::string &host, const std::string &url);

void										postHandler(Client & request);
