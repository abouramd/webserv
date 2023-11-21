#include "Client.hpp"
#include <ctime>

std::string getValue(const std::map<std::string, std::string> & headers, const std::string& key) {
    std::map<std::string, std::string>::const_iterator    it;

    it = headers.find(key);
    return it != headers.end() ? it->second : "";
}

void    Client::setEnv() {
    size_t  pos = fullPath.find_last_of("/");
    std::string script_name = pos != std::string::npos ? fullPath.substr(pos + 1) : fullPath;

    env["AUTH_TYPE"] = "null";
    env["REDIRECT_STATUS"] = "200";
    env["CONTENT_LENGTH"] = getValue(headers, "content-length");
    env["CONTENT_TYPE"] = getValue(headers, "content-type");
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["HTTP_ACCEPT"] = getValue(headers, "accept");
    env["HTTP_ACCEPT_CHARSET"] = getValue(headers, "accept-charset");
    env["HTTP_ACCEPT_ENCODING"] = getValue(headers, "accept-encoding");
    env["HTTP_ACCEPT_LANGUAGE"] = getValue(headers, "accept-language");
    env["HTTP_FORWARDED"] = getValue(headers, "forwarded");
    env["HTTP_HOST"] = host;
    env["HTTP_PROXY_AUTHORIZATION"] = getValue(headers, "proxy-authorization");
    env["HTTP_USER_AGENT"] = getValue(headers, "user-agent");
    env["PATH_INFO"] = fullPath;
    env["PATH_TRANSLATED"] = script_name;
    env["QUERY_STRING"] = query;
    env["REMOTE_HOST"] = getValue(headers, "host") != "" ? getValue(headers, "host") : "null";
    env["REMOTE_USER"] = getValue(headers, "authorization") != "" ? getValue(headers, "authorization") : "null";
    env["REQUEST_METHOD"] = method;
    env["SCRIPT_NAME"] = script_name;
    env["SERVER_NAME"] = host;
    env["SERVER_PROTOCOL"] = version;
    env["SERVER_SOFTWARE"] = "webserver v0.1";
    env["HTTP_COOKIE"] = getValue(headers, "cookie");
    env["REMOTE_ADDR"] = getValue(headers, "remote address");
    env["SERVER_PORT"] = server_port;
//    env["NCHOME"] = "#/path/to/netcdf";
}

Client::Client(int fd, std::ifstream *i, std::ofstream *o, std::map<int, std::string> e, std::map<int, std::string> ed)  : is(i), outfile(o) {
  this->write_f = 0;  
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
