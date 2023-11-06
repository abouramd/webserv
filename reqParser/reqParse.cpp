#include <cstdlib>
#include <vector>
#include "reqParse.hpp"
#include <algorithm>
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
        request.server = findServ(request.maxBodySize, serv, request.host, request.target);
        if (std::find(request.server->second.allow_method.begin(), request.server->second.allow_method.end(), request.method) == request.server->second.allow_method.end())
            throw 405;
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
//            if (isCgi(request))
//                handleCgi;
            if (!request.outfile->is_open())
                request.outfile->open(getFileName(request).c_str());
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
        request.statusCode = status;
        std::cout << "status code : " << status << std::endl;
        request.state = DONE;
    }
}
