#include "reqParse.hpp"


std::string getFileName(Client & request) {
    std::fstream content("content_type.txt");
    std::string     line;
    while (getline(content, line)) {
        if (request.headers["Content-Type"] == line.substr(0, line.find(':')))
            return "file" + line.substr(line.find(':') + 1);
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
        if (header.back() == '\r')
            header = header.substr(0, header.size() - 1);
        std::string key, value;
        key = header.substr(0, header.find(':'));
        value = header.substr(header.find(':') + 2);
        std::cout << key << ",>>>," << value << std::endl;
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
        if (request.headers.find("Transfer-Encoding") != request.headers.end()) {
            if (request.headers["Transfer-Encoding"] != "chunked" || request.headers.find("Content-length") != request.headers.end())
                throw 500;
        }
        else if (request.headers.find("Transfer-Encoding") != request.headers.end())
        request.contentLength = std::atoi(request.headers["Content-Length"].c_str());
    }
}

void    reqParser(Client & request, int sock) {
    try {
        int amount;

        amount = read(sock, request.buf, BUFF_SIZE);
        if (amount == 0) {
            request.state = CLOSE;
            return;
        }
        request.buffSize = amount;
        request.buf[request.buffSize] = 0;
        if (request.state == NOT_DONE)
            headersParsing(request);
        if (request.state == DONE_WITH_HEADERS && request.method == "POST") {
            if (!request.outfile.is_open())
                request.outfile.open(getFileName(request));
            if (request.chunkSize >= request.buffSize) {
                request.outfile.write(request.buf, request.buffSize);
                request.chunkSize -= request.buffSize;
                request.position = 2;
            }
            else
                bodyParser(request);
        }
        if (request.contentLength <= 0 || (request.state == DONE_WITH_HEADERS && request.method != "POST"))
            request.state = DONE;
    }
    catch (int status) {
        std::cout << "status is >> " << status << std::endl;
        request.state = DONE;
    }
}
