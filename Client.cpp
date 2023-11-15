#include "Client.hpp"
#include <ctime>

Client::Client(int fd, std::ifstream *i, std::ofstream *o, std::map<int, std::string> e, std::map<int, std::string> ed)  : is(i), outfile(o) {
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
