#include "Client.hpp"

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