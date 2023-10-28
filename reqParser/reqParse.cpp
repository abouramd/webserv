#include <cstdlib>
#include "reqParse.hpp"


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
    std::stringstream   ss(request.buf);

    ss >> request.method >> request.target >> request.version;
    if (request.method != "GET" && request.method != "POST" && request.method != "DELETE")
        throw 405;
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
        if (key.empty() || value.empty())
            throw 400;
        request.headers[key] = value;
    }
}

void    headersParsing(Client & request) {
    int pos = endFound(request.buf);
    if (pos != -1) {
        startHParsing(request);
        request.position = pos;
        request.state = DONE_WITH_HEADERS;
		if (request.headers.find("Host") == request.headers.end())
			throw 400;
        if (request.headers.find("Transfer-Encoding") != request.headers.end()) {
            if (request.headers["Transfer-Encoding"] != "chunked" || request.headers.find("Content-Length") != request.headers.end())
                throw 400;
        }
        else if (request.headers.find("Content-Length") == request.headers.end())
			throw 400;
        const char *ptr = request.headers["Content-Length"].c_str();
        request.contentLength = std::strtol(ptr, NULL, 10);
    }
}

void    reqParser(Client & request, int sock) {
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
            headersParsing(request);
        if (request.state == DONE_WITH_HEADERS && request.method == "POST") {
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
		request.response = generateResponse(status);
        std::cout << request.response << std::endl;
        request.outfile->close();
        request.state = DONE;
    }
}
