#include "Client.hpp"

Client::Client(int fd, std::ifstream *i, std::ofstream *o)  : is(i), outfile(o) {
    this->fd = fd;
    position = 0;
    chunkSize = 0;
    state = NOT_DONE;
}

void	Client::reset() {
	this->contentLength = 0;
	this->state = NOT_DONE;
	this->method.clear();
	this->target.clear();
	this->version.clear();
	this->host.clear();
	this->sizeDept.clear();
	this->headersBuf.clear();
	this->response.clear();
	this->headers.clear();
	this->outfile->close();
}