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
		key = Tools::toLower(key);
//        std::cout << key << ",>>>," << value << std::endl;
		if (key.empty() || value.empty() || request.headers.find(key) != request.headers.end())
			throw 400;
		if (key == "host")
			request.host = value;
		request.headers[key] = value;
	}
}

void	checkValidCharacters(const std::string & uri) {
    std::string validCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?# []@!$&'()*+,;=%");

	for (size_t i = 0; i < uri.size(); i++) {
		if (validCharacters.find(uri[i]) == std::string::npos)
			throw 400;
	}
}

void	parseUri( Client & request, std::string & path, std::string & query ) {
	size_t		pos;
	std::string	target(request.target.substr(request.location->first.size() - 1));

    checkValidCharacters(target);
    pos = target.find('?');
	if (pos != std::string::npos) {
		path = target.substr(0, pos);
		query = target.substr(pos + 1);
		Tools::decodeUri(path);
	}
	else {
		path = target;
		Tools::decodeUri(path);
	}
}

void	targetChecker( Client & request ) {
	std::string	path, query;
	bool		r(false), w(false);

	parseUri(request, path, query);
	request.path = path;
	request.query = query;
	request.fullPath = request.location->second.root + path;
	std::cout << request.fullPath << ">>>" << std::endl;
	if (!Tools::pathExists(request.fullPath.c_str(), request.isDir, r, w))
		throw 404;
	if (!r)
		throw 403;
	if (request.method == "POST" && request.isDir && request.location->second.index.size()) {
		std::string	newPath;
		bool		isDir;

		for (size_t	i = 0; i < request.location->second.index.size(); i++) {
			isDir = false;
			r = false;
			w = false;
			newPath = request.fullPath + request.location->second.index[0];
			if (Tools::pathExists(newPath.c_str(), isDir, r, w) && r) {
				request.fullPath = newPath;
				request.isDir = isDir;
				break;
			}
			std::cout << request.isDir << "::::" << std::endl;
		}
	}
}

void    headersParsing(Client & request, std::vector<Server>& serv) {
	int pos = endFound(request.buf);

	if (pos != -1) {
        request.position = pos;
        startHParsing(request);
		request.location = findServ(request, serv, request.host, request.target);
		if (std::find(request.location->second.allow_method.begin(), request.location->second.allow_method.end(), request.method) == request.location->second.allow_method.end())
			throw 405;
		if (request.target[0] != '/' || request.target.size() > 2048)
			throw 400;
		if (request.host.empty())
			throw 400;
        if (request.headers["content-type"].find("multipart/form-data; boundary=") == 0) {
            request.isBound = true;
            request.boundary = request.headers["content-type"].substr(30);
//            std::cout << request.boundary << ":::::" << std::endl;
            if (request.headers.find("transfer-encoding") != request.headers.end())
                throw 501;
        }
		if (request.method == "POST") {
			if (request.headers.find("transfer-encoding") != request.headers.end()) {
				if (request.headers["transfer-encoding"] != "chunked")
					throw 501;
				if (request.headers.find("content-length") != request.headers.end())
					throw 400;
			}
			else if (request.headers.find("content-length") == request.headers.end())
				throw 400;
		}
		targetChecker(request);
		request.state = DONE_WITH_HEADERS;
		request.contentLength = std::strtol(request.headers["content-length"].c_str(), NULL, 10);
		if (request.contentLength > request.maxBodySize)
			throw 413;
	}
	else
		request.headersBuf += request.buf;
}

void    reqParser(Client & request, int sock, std::vector<Server>& serv) {
    try {
		int amount = 1024;
//        std::ofstream fll("hello.hello");
//        while (true) {
//            std::cout << amount << std::endl;
//            amount = recv(sock, request.buf, BUFF_SIZE, 0);
//            fll.write(request.buf, amount);
//            fll.flush();
//            request.buf[amount] = 0;
//            std::cout << request.buf << std::endl;
//        }

//        fll.close();
//        exit(444);
        amount = read(sock, request.buf, BUFF_SIZE);
        if (amount == 0) {
			request.state = CLOSE;
			throw 200;
		}
		request.buffSize = amount;
		request.buf[request.buffSize] = 0;
		if (request.state == NOT_DONE)
			headersParsing(request, serv);
        if (request.state == DONE_WITH_HEADERS && request.method != "POST")
            throw 200;
        if (request.state == DONE_WITH_HEADERS && request.method == "POST")
            postHandler(request);
	}
	catch (int status) {
        if (request.method == "POST" && (status == 200 || status == 201) && request.isCgi) {
			Cgi	cgi(request);

			cgi.executeCgi();
		}
        else if (!request.isDir)
            request.method = "GET";
		request.statusCode = status;
		std::cout << "status code : " << status << std::endl;
        request.outfile->close();
		if (request.state != CLOSE)
			request.state = DONE;
	}
}
