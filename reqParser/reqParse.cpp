#include "Cgi.hpp"

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
	std::string	filename(request.location->second.root + request.target);

	if (request.target[0] != '/')
		throw 400;
	if (Cgi::fileExists(filename.c_str())) {
		if (Cgi::hasReadPermission(filename.c_str())) {
			if (request.location->second.cgi.first) {
				std::string extension;

				getExtension(request.target, extension);
				if (!extension.empty()) {
					std::map<std::string, std::string>::iterator it = request.location->second.cgi.second.find(extension);

					if (it != request.location->second.cgi.second.end()) {
						request.cgiScript = it->second;
						request.isCgi = true;
					}
					else if (request.method == "POST")
						throw 404;
				}
			}
		} else
			throw 403;
	} else
		throw 404;
}

void    headersParsing(Client & request, std::vector<Server>& serv) {
    int pos = endFound(request.buf);

    if (pos != -1) {
        startHParsing(request);
        if (request.host.empty())
            throw 400;
        if (request.method == "POST") {
			if (request.headers.find("Transfer-Encoding") != request.headers.end() && request.headers["Transfer-Encoding"] != "chunked")
				throw 400;
            else if (request.headers.find("Transfer-Encoding") == request.headers.end() && request.headers.find("Content-Length") == request.headers.end())
                throw 400;
        }
        request.location = findServ(request.maxBodySize, serv, request.host, request.target);
        if (std::find(request.location->second.allow_method.begin(), request.location->second.allow_method.end(), request.method) == request.location->second.allow_method.end())
            throw 405;
        targetChecker(request);
        request.position = pos;
        request.state = DONE_WITH_HEADERS;
        const char *ptr = request.headers["Content-Length"].c_str();
        request.contentLength = std::strtol(ptr, NULL, 10);
        if (request.contentLength > request.maxBodySize)
            throw 413;
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
		if (request.method == "POST" && (status == 200 || status == 201) && request.isCgi) {
			Cgi	cgi(request);

			cgi.executeCgi();
		}
		std::cout << ">>>>>>" << std::endl;
		request.statusCode = status;
		std::cout << "status code : " << status << std::endl;
        request.state = DONE;
    }
}
