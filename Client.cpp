#include "Client.hpp"

Client::Client(int fd, std::ifstream *i, std::ofstream *o)  : is(i), outfile(o) {
    this->fd = fd;
	this->isCgi = false;
    this->position = 0;
    this->chunkSize = 0;
    this->state = NOT_DONE;
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
	this->headers.clear();
	this->outfile->close();
	this->isCgi = false;
}
