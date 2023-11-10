#include "Client.hpp"
#include <ctime>

Client::Client(int fd, std::ifstream *i, std::ofstream *o)  : is(i), outfile(o) {
    this->fd = fd;
	this->isCgi = false;
	this->isDir = false;
    this->isBound = false;
    this->beenThere = false;
    this->boundState = HEAD;
    this->position = 0;
    this->chunkSize = 0;
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
	this->sizeDept.clear();
	this->headersBuf.clear();
	this->headers.clear();
	this->outfile->close();
	this->isCgi = false;
}
