#include "Socket.hpp"
#include "Location.hpp"
#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <string>

#ifdef __APPLE__
#include <sys/_endian.h>
#endif // !__APPLE__
#ifndef __APPLE__
#include <endian.h>
#endif // !__APPLE__

#include <sys/socket.h>

Socket::Socket() {
  this->Socket_fd = -1;
  this->port = -1;
  this->server.sin_family = AF_INET;
}

Socket::~Socket() {}


// void  Socket::setFd(const int fd)
// {
//   if (fd != -1)
//     throw std::string("fd socket");
//   this->Socket_fd = fd;
// }
//
int Socket::getFd() const { return this->Socket_fd; }

void Socket::setHost(const std::string h) {
  if (!this->host.empty())
    throw std::string("host socket");
  this->host = h;
  inet_aton(h.c_str(), &this->server.sin_addr);
}

std::string Socket::getHost() const { return this->host; }

in_addr Socket::getSinAddr() const { return this->server.sin_addr; }

void Socket::setPort(const int p) {
  // std::cout << "port -> " << p << std::endl;
  if (this->port != -1)
    throw std::string("port socket");
  this->port = p;
  this->server.sin_port = htons(p);
}

int Socket::getPort() const { return this->port; }

int Socket::getSinPort() const { return this->server.sin_port; }

void Socket::connectASocket() {
  int opt = 1;

  if (this->port == -1 || this->host.empty())
    throw std::string("port or host not set");

  if ((this->Socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    throw std::string("Error: falid to create a socket.");

  if (setsockopt(this->Socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    throw std::string("Error: setsockopt falid.");

  if (bind(this->Socket_fd, (sockaddr *)&this->server, sizeof(this->server)) == -1)
    throw std::string("Error: falid to bind a socket to a port.");

  if (listen(this->Socket_fd, SOMAXCONN) == -1)
    throw std::string("Error: falid to listen a socket to a port.");
  
  this->port = ntohs(this->server.sin_port);
  this->host = inet_ntoa(this->server.sin_addr);
  std::cout << GREEN << this->host << ":" << this->port << DFL << " Socket fd is " << BLUE << this->Socket_fd << DFL << std::endl;
}

void Socket::check_server_name(std::vector<std::string> &sn) {
  for (std::vector<std::string>::iterator it = sn.begin(); it != sn.end();
       it++) {
    for (std::vector<std::string>::iterator s = this->server_name.begin();
         s != this->server_name.end(); s++)
      if (*it == *s)
        throw std::string("Error: `" + *s +
                          "` in two server that have the same port and host.");
  }
  this->server_name.insert(this->server_name.begin(), sn.begin(), sn.end());
}
