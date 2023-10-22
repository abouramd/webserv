#include "Config.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/_endian.h>
#include <sys/socket.h>
#include <vector>

Config::Config()
{

}

// Config::Config(const Config& obj)
// {
//   (void) obj;
// }
//
// Config& Config::operator=(const Config& obj)
// {
//   (void) obj;
//   return *this;
// }
//
Config::~Config()
{

}

void Config::pars(int ac, char **av)
{
  this->init_data(ac, av);
  this->read_data();
  this->creat_socket();
}


void Config::init_data(int ac, char **av)
{
  if (ac != 2)
    throw std::string("Error: number of args (the programe should take ane param).");
  
  this->filename = av[1];
  
  this->file.open(av[1]);
  if (!this->file.is_open())
    throw "Error" + std::string(strerror(errno)) + " (" + this->filename + ").";
}

void Config::read_data()
{
  std::string buffer;

  while (ft_read(this->file, buffer))
  {
    Server serv;
    if (buffer == "server {")
      serv.init_data(this->file);
    else
      throw "Error: " + buffer; 
    this->servers.push_back(serv);
    
    // std::cout << "=> " << this->servers[0].getPort().size() << std::endl;
     // std::cout << "port " <<  serv.getPort().size() << std::endl;
  }
  this->file.close();
}

void Config::creat_socket()
{
  std::vector<Server>::iterator it_sev = this->servers.begin();
  while (it_sev != this->servers.end())
  {
    sockaddr_in addr;
    inet_aton(it_sev->getHost().c_str(), &addr.sin_addr);
    std::vector<std::string> ports = it_sev->getPort();
    // std::cout << "number of servers is " << this->servers.size() << " " << it_sev->getPort().size() << std::endl;
    std::vector<std::string>::iterator it_port = ports.begin();
    while (it_port != ports.end())
    {
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


void Config::add_socket(sockaddr_in &addr, Server& sev, int& port)
{
  std::vector<Socket>::iterator it = this->socket.begin();
  while (it != this->socket.end())
  {
    if (it->getSinPort() == addr.sin_port)
    {
      if (it->getSinAddr().s_addr != addr.sin_addr.s_addr)
        throw std::string("Error: this port " + std::to_string(port) + " is used with " + inet_ntoa(it->getSinAddr()) + " and " + inet_ntoa(addr.sin_addr));
      it->serv.push_back(sev);
      return;
    }
    it++;
  }
  Socket obj;
  std::cout << "add a socket" << std::endl;
  obj.setHost(sev.getHost());
  obj.setPort(port);
  obj.connectASocket();
  obj.serv.push_back(sev);
  this->socket.push_back(obj);
}

std::vector<Socket>& Config::get_socket()
{
  return this->socket;
}


