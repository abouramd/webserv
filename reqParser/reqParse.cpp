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
//        std::cout << key << ",>>>," << value << std::endl;
		if (key.empty() || value.empty() || request.headers.find(key) != request.headers.end())
			throw 400;
		if (key == "Host")
			request.host = value;
		request.headers[key] = value;
	}
}

bool	getExtension(std::string & target, std::string & extension) {
	size_t	pos;

	for (int i = target.size() - 1; i >= 0; i--) {
		if (target[i] == '.') {
			pos = target.find('/', i);
			if (pos != std::string::npos)
				extension = target.substr(i, pos - i);
			else
				extension = target.substr(i);
			return true;
		}
	}
	return false;
}

void	checkValidCharacters(const std::string & path, const std::string & query, bool withQuery) {
	for (size_t i = 0; i < path.size(); i++) {
		if (!std::isalnum(path[i]) && std::string("/-_.").find(path[i]) == std::string::npos)
			throw 400;
	}
	for (size_t i = 0; withQuery && i < query.size(); i++) {
		if (!std::isalnum(query[i]) && std::string("$-_.+!*'(),").find(query[i]) == std::string::npos)
			throw 400;
	}
}

void	parseUri( Client & request, std::string & path, std::string & query ) {
	size_t	pos;

	pos = request.target.find('?');
	if (pos != std::string::npos) {
		path = request.target.substr(0, pos);
		query = request.target.substr(pos + 1);
		Tools::decodeUri(path);
		checkValidCharacters(path, query, true);
	}
	else {
		path = request.target;
		Tools::decodeUri(path);
		checkValidCharacters(path, query, false);
	}
}

void	targetChecker( Client & request ) {
	std::string	filename, path, query;
	bool		isDir(false), r(false), w(false);

	parseUri(request, path, query);
	filename = request.location->second.root + path;
	if (Tools::pathExists(filename.c_str(), isDir, r, w)) {
		if (r) {
			if (!isDir && request.location->second.cgi.first) {
				std::string extension;

				if (getExtension(request.target, extension)) {
					std::map<std::string, std::string>::iterator it = request.location->second.cgi.second.find(extension);

					if (it != request.location->second.cgi.second.end()) {
						request.cgiScript = it->second;
						request.isCgi = true;
					} else if (request.method == "POST")
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
			if (request.headers.find("Transfer-Encoding") != request.headers.end()) {
        std::cout << "heeere" << std::endl;
				if (request.headers["Transfer-Encoding"] != "chunked")
					throw 501;
				if (request.headers.find("Content-Length") != request.headers.end())
					throw 400;
			}
			else if (request.headers.find("Content-Length") == request.headers.end())
				throw 400;
		}
		request.location = findServ(request.maxBodySize, serv, request.host, request.target);
		if (std::find(request.location->second.allow_method.begin(), request.location->second.allow_method.end(), request.method) == request.location->second.allow_method.end())
			throw 405;
		if (request.target[0] != '/' || request.target.size() > 2048)
			throw 400;
		request.target = request.target.substr(request.location->first.size());
		targetChecker(request);
		request.position = pos;
		request.state = DONE_WITH_HEADERS;
		const char *ptr = request.headers["Content-Length"].c_str();
		request.contentLength = std::strtol(ptr, NULL, 10);
		std::cout << request.maxBodySize << ",,,," << request.contentLength << std::endl;
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
    std::cout << "amount .> " << amount << std::endl;
		if (amount == 0) {
			request.state = CLOSE;
			throw 200;
		}
		request.buffSize = amount;
		request.buf[request.buffSize] = 0;
		if (request.state == NOT_DONE)
			headersParsing(request, serv);
		if (request.state == DONE_WITH_HEADERS && request.method == "POST")
			postHandler(request);
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
