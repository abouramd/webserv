#include "Config.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#ifndef __APPLE__
#include <endian.h>
#endif // !__APPLE__
#include <cstring>
#include <sys/socket.h>
#include <vector>

Config::Config() {
  FD_ZERO(&this->err_fd);
  FD_ZERO(&this->read_fd);
  FD_ZERO(&this->write_fd);
}

Config::~Config() {}

int Config::add_client(Socket &sock) {
  int fd = accept(sock.getFd(), NULL, NULL);
  if (fd < 0)
    return -1;
  map_files[fd] = make_pair(new std::ifstream, new std::ofstream);
  sock.client.push_back(Client(fd, map_files[fd].first, map_files[fd].second, sock.serv[0].error_page, sock.serv[0].error_page_dfl));
  return 0;
}

void Config::rm_client(Socket& sock, int index)
{
  if (index >= 0 && index < (int)sock.client.size())
  {
    close(sock.client[index].fd);
    if (sock.client[index].cgiFileName != "")
      std::remove(sock.client[index].cgiFileName.c_str());
    map_files[sock.client[index].fd].first->close();
    map_files[sock.client[index].fd].second->close();
    delete map_files[sock.client[index].fd].first;
    delete map_files[sock.client[index].fd].second;
    map_files.erase(sock.client[index].fd);
    sock.client.erase(sock.client.begin() + index);
  }
}

void Config::pars(int ac, char **av) {
  this->init_data(ac, av);
  this->read_data();
  if (this->servers.empty())
    throw std::string("Error: empthy file, no servers founded in the config file.");
  this->creat_socket();
}

void Config::init_data(int ac, char **av) {
  if (ac == 1)
    this->filename = "default";
  else if (ac == 2)
    this->filename = av[1];
  else
    throw std::string("Error: number of args (the programe should take ane param).");

  this->file.open(av[1]);
  if (!this->file.is_open())
    throw "Error" + std::string(strerror(errno)) + " (" + this->filename + ").";
}

void Config::read_data() {
  std::string buffer;

  while (ft_read(this->file, buffer)) {
    Server serv;
    if (buffer == "server {")
      serv.init_data(this->file);
    else
      throw "Error: " + buffer;
    serv.check();
    this->servers.push_back(serv);

    // std::cout << "=> " << this->servers[0].getPort().size() << std::endl;
    // std::cout << "port " <<  serv.getPort().size() << std::endl;
  }
  this->file.close();
}

void Config::creat_socket() {
  std::vector<Server>::iterator it_sev = this->servers.begin();
  while (it_sev != this->servers.end()) {
    sockaddr_in addr;
    inet_aton(it_sev->getHost().c_str(), &addr.sin_addr);
    std::vector<std::string> ports = it_sev->getPort();
    // std::cout << "number of servers is " << this->servers.size() << " " <<
    // it_sev->getPort().size() << std::endl;
    std::vector<std::string>::iterator it_port = ports.begin();
    while (it_port != ports.end()) {
      std::stringstream str;
      int p;

      str << *it_port;
      str >> p;

      addr.sin_port = htons(p);

      this->add_socket(addr, *it_sev, p);

      it_port++;
    }
    it_sev++;
  }
}

void Config::add_socket(sockaddr_in &addr, Server &sev, int &port) {
  std::vector<Socket>::iterator it = this->socket.begin();
  std::stringstream ss;
  while (it != this->socket.end()) {
    if (it->getSinPort() == addr.sin_port) {
      if (it->getSinAddr().s_addr != addr.sin_addr.s_addr) {
        ss << port;
        throw std::string("Error: this port " + ss.str() + " is used with " +
                          inet_ntoa(it->getSinAddr()) + " and " +
                          inet_ntoa(addr.sin_addr));
      }
      it->check_server_name(sev.server_name);
      it->serv.push_back(sev);
      return;
    }
    it++;
  }
  Socket obj;
  obj.check_server_name(sev.server_name);
  std::cout << "add a socket" << std::endl;
  obj.setHost(sev.getHost());
  obj.setPort(port);
  obj.connectASocket();
  obj.serv.push_back(sev);
  this->socket.push_back(obj);
}

