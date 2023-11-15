#include "Client.hpp"
#include <ctime>

std::string getValue(Client & request, const std::string& key) {
    std::map<std::string, std::string>::iterator    it;

    it = request.headers.find(key);
    return it != request.headers.end() ? it->second : "";
}

void    setEnv(Client & request) {
    request.env["AUTH_TYPE"] = "null";
    request.env["REDIRECT_STATUS"] = "200";
    request.env["CONTENT_LENGTH"] = getValue(request, "content-length");
    request.env["CONTENT_TYPE"] = getValue(request, "content-type");
    request.env["GATEWAY_INTERFACE"] = "CGI/1.1";
    request.env["HTTP_ACCEPT"] = getValue(request, "accept");
    request.env["HTTP_ACCEPT_CHARSET"] = getValue(request, "accept-charset");
    request.env["HTTP_ACCEPT_ENCODING"] = getValue(request, "accept-encoding");
    request.env["HTTP_ACCEPT_LANGUAGE"] = getValue(request, "accept-language");
    request.env["HTTP_FORWARDED"] = getValue(request, "forwarded");
    request.env["HTTP_HOST"] = request.host;
    request.env["HTTP_PROXY_AUTHORIZATION"] = getValue(request, "proxy-authorization");
    request.env["HTTP_USER_AGENT"] = getValue(request, "user-agent");
    request.env["PATH_INFO"] = request.target;
    request.env["PATH_TRANSLATED"] = request.fullPath;
    request.env["QUERY_STRING"] = request.query;
    request.env["REMOTE_HOST"] = "null";
    request.env["REMOTE_USER"] = getValue(request, "authorization") != "" ? getValue(request, "authorization") : "null";
    request.env["REQUEST_METHOD"] = request.method;
    request.env["SCRIPT_NAME"] = request.fullPath;
    request.env["SERVER_NAME"] = request.host;
    request.env["SERVER_PROTOCOL"] = request.version;
    request.env["SERVER_SOFTWARE"] = "webserver v0.1";
    request.env["HTTP_COOKIE"] = getValue(request, "cookies");
//    request.env["REMOTE_ADDR"] = ipADD;
//    request.env["SERVER_PORT"] = port;
//    request.env["NCHOME"] = "#/path/to/netcdf";
}

Client::Client(int fd, std::ifstream *i, std::ofstream *o, std::map<int, std::string> e, std::map<int, std::string> ed)  : is(i), outfile(o) {
    setEnv(*this);
    this->error_page = e;
	this->error_page_dfl = ed;
	this->fd = fd;
	this->isCgi = false;
	this->isDir = false;
	this->isBound = false;
    this->beenThere = false;
    this->boundState = AT_START;
	this->pState = METHOD;
    this->chState = SIZE;
    this->position = 0;
    this->contentLength = 0;
    this->chunkSizeNum = 0;
    this->state = NOT_DONE;
    this->request_time = std::time(NULL);
}

void	Client::reset() {
	this->contentLength = 0;
	this->state = NOT_DONE;
	this->method.clear();
	this->target.clear();
	this->version.clear();
	this->host.clear();
	this->headers.clear();
	this->outfile->close();
	this->isCgi = false;
}
