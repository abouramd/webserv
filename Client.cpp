#include "Client.hpp"

Client::Client(int fd, std::ifstream* i, std::ofstream* o) : is(*i), os(*o)
{
  this->fd = fd;
}
Client::Client(const Client& obj) : is(obj.is), os(obj.os)
{
  this->fd = obj.fd;
}
// Client::Client(){}
//
// Client::Client(const Client &obj)
// {
//   std::cout << "copy" << std::endl;
//   this->fd = obj.fd;
// }
//
// Client& Client::operator=(const Client &obj)
// {
//   std::cout << "copy" << std::endl;
//   this->fd = obj.fd;
//   return *this;
// }
