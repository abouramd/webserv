#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <strings.h>
#include <sys/wait.h>
#include <vector>
#include "reqParse.hpp"
#include <algorithm>
#include <unistd.h>
#include <wait.h>
//bool    isCgi(Client & request) {
//    std::stringstream   target(request.target);
//    std::string         line;
//
//    while (getline(target, line, '/'));
//
//    for (int i = 0; i < line.size(); i++) {
//        if (line[i] == '.' && i + 3 == line.size() - 1 && line.substr(i + 1, 3) == "php")
//            return true;
//    }
//    return false;
//}
//
//void    handleCgi(Client & request) {
//    std::ifstream   cgiFile(request.target);
//    char            **env;
//    const char      args[] = {request.target, NULL};
//    int             pid;
//
//    if (!cgiFile.is_open())
//        throw 404;
//    env = createEnv(request);
//    pid = fork();
//    if (!pid)
//        execve(request.target, args, env);
//}

std::string getFileName(Client & request) {
    std::fstream content("content_type.txt");
    std::string     line;
	std::string		res;

    while (getline(content, line)) {
        if (request.headers["Content-Type"] == line.substr(0, line.find(':'))) {
			res = "file" + line.substr(line.find(':') + 1);
			return res;
		}
    }
    throw 415;
}

int    endFound( const char *buf ) {
    std::string str(buf);
    size_t        pos;

    pos = str.find("\n\n");
    if (pos != std::string::npos)
        return pos + 2;
    pos = str.find("\r\n\r\n");
    if (pos != std::string::npos)
        return pos + 4;
    return -1;
}

void    startHParsing(Client & request) {
    std::string header;
    std::stringstream   ss;

	ss << request.headersBuf;
	ss.write(request.buf, request.buffSize);
    ss >> request.method;
    if (request.method != "GET" && request.method != "POST" && request.method != "DELETE")
        throw 405;
    ss >> request.target;
    if (request.target.empty())
        throw 400;
    ss >> request.version;
    if (request.version != "HTTP/1.1")
        throw 400;
    if (request.version != "HTTP/1.1")
        throw 505;
    if (ss.peek() == '\r')
        ss.ignore(2);
    else
        ss.ignore(1);
    std::cout << request.method << ", " << request.target << ", " << request.version << std::endl;
    while (std::getline(ss, header) && !header.empty() && header != "\r") {
        if (header[header.size() - 1] == '\r')
            header = header.substr(0, header.size() - 1);
        std::string key, value;
        key = header.substr(0, header.find(':'));
        value = header.substr(header.find(':') + 2);
//        std::cout << key << ",>>>," << value << std::endl;
        if (key.empty() || value.empty() || request.headers.find(key) != request.headers.end())
            throw 400;
        if (key == "Host")
            request.host = value;
        request.headers[key] = value;
    }
}

void  executeCgi( Client & request ) {
  int           pid;
  char          *args[3];
  FILE          *in;
  FILE          *out;
  std::stringstream	ss;

  ss << "_tmp/" << rand() << "_cgi_out.tmp";
  args[0] = new char[request.cgiScript.size() + 1];
  args[1] = new char[request.location->second.root.size() + request.target.size() + 1];
  bzero(args[0], request.cgiScript.size() + 1);
  bzero(args[1], request.location->second.root.size() + request.target.size() + 1);
  strcpy(args[0], request.cgiScript.c_str());
  strcpy(args[1], (request.location->second.root + request.target).c_str());
  args[2] = NULL;
  request.outfile->close();
  pid = fork();
  if (!pid) {
	  std::cerr << args[0] << ", " << args[1] << std::endl;
	  in = freopen(request.cgiFileName.c_str(), "r", stdin);
	  out = freopen(std::string(ss.str()).c_str(), "w", stdout);
	  (void)in;
	  (void)out;
	  execve(args[0], args, NULL);
  }
  else {
	  waitpid(pid, NULL, 0);
	  request.cgiFileName = ss.str();
  }
}

bool	getExtension(std::string & target, std::string & extension) {
	for (int i = target.size() - 1; i >= 0; i--) {
		if (target[i] == '.') {
			extension = target.substr(i);
			return 1;
		}
	}
	return 0;
}

void	targetChecker( Client & request ) {
	if (request.target[0] != '/')
		throw 400;

	if (request.location->second.cgi.first) {

		std::string extension;
		getExtension(request.target, extension);

		if (!extension.empty()) {
      std::map<std::string, std::string>::iterator it = request.location->second.cgi.second.find(extension);

			if (it != request.location->second.cgi.second.end()) {
        request.cgiScript = it->second;
        request.isCgi = true;
      }
			else
				throw 404;
		}
	}
}

void    headersParsing(Client & request, std::vector<Server>& serv) {
    int pos = endFound(request.buf);

    if (pos != -1) {
        startHParsing(request);
        if (request.host.empty())
            throw 400;
        if (request.method == "POST" && request.headers.find("Transfer-Encoding") != request.headers.end()) {
            if (request.headers["Transfer-Encoding"] != "chunked" || request.headers.find("Content-Length") != request.headers.end())
                throw 400;
        }
        else if (request.method == "POST" && request.headers.find("Content-Length") == request.headers.end())
            throw 400;
        request.location = findServ(request.maxBodySize, serv, request.host, request.target);
        if (std::find(request.location->second.allow_method.begin(), request.location->second.allow_method.end(), request.method) == request.location->second.allow_method.end())
            throw 405;
        targetChecker(request);
        request.position = pos;
        request.state = DONE_WITH_HEADERS;
        const char *ptr = request.headers["Content-Length"].c_str();
        request.contentLength = std::strtol(ptr, NULL, 10);
        if (request.contentLength > request.maxBodySize)
            request.contentLength = request.maxBodySize;
    }
    else
		request.headersBuf += request.buf;
}

void    reqParser(Client & request, int sock, std::vector<Server>& serv) {
  try {
        int amount;

        amount = read(sock, request.buf, BUFF_SIZE);
        if (amount == 0) {
            request.state = CLOSE;
            throw 200;
        }
        request.buffSize = amount;
        request.buf[request.buffSize] = 0;
        if (request.state == NOT_DONE)
            headersParsing(request, serv);
        if (request.state == DONE_WITH_HEADERS && request.method == "POST") {
            if (!request.outfile->is_open()) {
				if (request.isCgi) {
                  std::stringstream	ss;

				  ss << rand();
				  request.cgiFileName = "_tmp/" + ss.str() + "_cgi_in.tmp";
				  request.outfile->open(request.cgiFileName.c_str());
              }
              else {
				  std::string		extension = FileType::getExt(request.headers["Content-Type"]);
				  std::stringstream	ss;

				  ss << request.location->second.root << rand() << "_file." << extension;
				  request.outfile->open(std::string(ss.str()).c_str());
			  }
            }
            if (request.chunkSize >= request.buffSize) {
                request.outfile->write(request.buf, request.buffSize);
                request.chunkSize -= request.buffSize;
                request.position = 2;
            }
            else
                bodyParser(request);
        }
        if (request.state == DONE_WITH_HEADERS && request.method != "POST")
            throw 200;
    }
    catch (int status) {
		if (request.isCgi)
			executeCgi(request);
		request.statusCode = status;
		std::cout << "status code : " << status << std::endl;
        request.state = DONE;
    }
}
