#include "Server.hpp"
#include "Location.hpp"
#include <cctype>
#include <climits>
#include <cstddef>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/errno.h>
#include <sys/stat.h>
#include <vector>
#define UINT16_MAX 65535
//set max body size 
// check duplicate 

Server::Server() {
    max_body_size = 1048576;
   
    for (size_t i = 0; i < 5; i++)
      this->dup[i] = false;

    this->key[0] = "host";
    this->key[1] = "listen";
    this->key[2] = "server_name";
    this->key[3] = "location";
    this->key[4] = "max_body_size";
    this->key[5] = "";
    this->key[6] = "";
    this->key[7] = "";
    this->key[8] = "";
    this->key[9] = "";
    this->key[10] = "";

    ptr[0] = &Server::set_host;
    ptr[1] = &Server::set_port;
    ptr[2] = &Server::set_server_name;
    ptr[3] = &Server::set_location;
    ptr[4] = &Server::set_max_body;
    ptr[5] = NULL;
    ptr[6] = NULL;
    ptr[7] = NULL;
    ptr[8] = NULL;
    ptr[9] = NULL;
    ptr[10] = NULL;
    ptr[11] = NULL;
}

// Server::Server(const Server& obj)
// {
//   (void) obj;
// }
//
// Server& Server::operator=(const Server& obj)
// {
//   (void) obj;
//   return *this;
// }

Server::~Server()
{

}

void Server::find_key(const std::string &str)
{
  std::vector<std::string> vec = my_split(str);
  size_t index = 0;

  while (!this->key[index].empty())
  {
    if (vec[0] == this->key[index])
    {
      (this->*ptr[index])(str);
      return;
    }
    index++;
  }
  throw std::string("Error: unknowing `" + str + "`.");
}

void Server::init_data(std::ifstream &file)
{
  std::string buffer;

  this->file = &file;
  while (ft_read(file, buffer))
  {
    if (buffer == "}")
    {
      // std::cout << "port " <<  this->port.size() << std::endl;
      return;
    }
    this->find_key(buffer);
  }
  throw std::string("Error: unclosed bracket");
}

void Server::set_host(const std::string &str)
{
  in_addr addr;

  if (this->dup[0])
    throw std::string("Error: The host is duplicated `" + str + "`.");
  this->dup[0] = true;
  
  std::vector<std::string> vec = my_split(str);
  
  if (vec.size() != 2)
    throw std::string("Error: number of args (" + str + ")");
  
  if (inet_aton(vec[1].c_str(), &addr) == 0)
    throw std::string("Error: invalid Ip address (" + str + ")");

  this->host = vec[1];
}

long Server::str_to_int(std::string &str, long m)
{
  long buff = 0, pre = 0;
  std::string::iterator it = str.begin();

  if (it == str.end())
    return -1;

  while (it != str.end())
  {
    if (!std::isdigit(*it))
      return -1;
    buff = buff * 10 + *it - '0';
    if (buff > m || buff < pre)
      return -1;
    pre = buff;
    it++;
  }
  return buff;
}

void Server::set_port(const std::string &str)
{
  int nbr = 0;

  std::vector<std::string> vec = my_split(str);
  
  if (vec.size() < 2)
    throw std::string("Error: number of args (" + str + ")");

  std::vector<std::string>::iterator it = vec.begin() + 1;
  while (it != vec.end())
  {
    nbr = (int)str_to_int(*it, UINT16_MAX);
    if (nbr == -1)
      throw std::string("Error: invalid port `" + *it + "` in (" + str + ").");
    this->port.push_back(*it);
    it++;
  }
}

// void Server::set_root(const std::string &str)
// {
//   struct stat sb;
//
//   if (!this->root.empty())
//     throw std::string("Error: The root is duplicated.");
//   
//   std::vector<std::string> vec = my_split(str);
//   
//   if (vec.size() != 2)
//     throw std::string("Error: number of args (" + str + ")");
//   
//   if (!(stat(vec[1].c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)))
//     throw std::string("Error: root Path `" + std::string(strerror(errno)) + "` (" + str + ")");
//
//   this->root = vec[1];
// }
//
// void Server::set_index(const std::string &str)
// {
//   std::vector<std::string> vec = my_split(str);
//   
//   if (vec.size() < 2)
//     throw std::string("Error: number of args (" + str + ")");
//   
//   std::vector<std::string>::iterator it = vec.begin() + 1;
//   while (it != vec.end())
//   {
//     this->index.push_back(*it);
//     it++;
//   }
// }
//   
// void Server::set_auto_index(const std::string &str)
// {
//   if (0)
//     throw std::string("Error: The auto_index is duplicated.");
//   
//   std::vector<std::string> vec = my_split(str);
//   
//   if (vec.size() != 2)
//     throw std::string("Error: number of args (" + str + ")");
//   
//   if (vec[1] == "on")
//     this->auto_index = true;
//   else if (vec[1] == "off")
//     this->auto_index = false;
//   else
//     throw std::string("Error: unknowing option `" + vec[1] + "` in (" + str + ")");
// }
//
void Server::set_server_name(const std::string &str)
{
  std::vector<std::string> vec = my_split(str);
  
  if (vec.size() < 2)
    throw std::string("Error: number of args (" + str + ")");
  
  std::vector<std::string>::iterator it = vec.begin() + 1;
  while (it != vec.end())
  {
    this->server_name.push_back(*it);
    it++;
  }
}
//
// void Server::set_method(const std::string &str)
// {
//    std::vector<std::string> vec = my_split(str);
//   
//   if (vec.size() < 2)
//     throw std::string("Error: number of args (" + str + ")");
//   
//   std::vector<std::string>::iterator it = vec.begin() + 1;
//   
//   while (it != vec.end())
//   {
//     if (*it == "GET")
//       this->allow_method.push_back(GET);
//     else if (*it == "POST")
//       this->allow_method.push_back(POST);
//     else if (*it == "DELET")
//       this->allow_method.push_back(DELETE);
//     else
//       throw std::string("Error: unknowing method `" + *it + "` in (" + str + ")");
//     it++;
//   }
// }

void Server::set_location(const std::string &str)
{
  Location obj;
  std::string location_name;

  std::vector<std::string> vec = my_split(str);
  
  if (vec.size() != 3)
    throw std::string("Error: number of args (" + str + ")");

  if (vec[2] != "{")
    throw std::string("Error: bracket not found `" + vec[2] + "` (" + str + ")");

  location_name = vec[1];
  
  if (location_name[0] != '/')
    throw std::string("Error: This location name `" + location_name + "` doesn't start with '/' (" + str + ")");

  if (this->location.find(location_name) != this->location.end())
    throw std::string("Error: This location `" + location_name + "` is already used (" + str + ")");
  
  obj.init_data(*this->file);
  obj.check();
  this->location[location_name] = obj;
}

void Server::set_max_body(const std::string &str)
{
  long nbr = 0;

  if (this->dup[1])
    throw std::string("Error: max_body_size is duplicated `" + str + "`.");
  this->dup[1] = true;
  
  std::vector<std::string> vec = my_split(str);
  
  if (vec.size() != 2)
    throw std::string("Error: number of args (" + str + ")");

  nbr = str_to_int(vec[1], LONG_MAX);
  if (nbr == -1)
    throw std::string("Error: invalid port `" + vec[1] + "` in (" + str + ").");
  this->max_body_size = nbr;
}

std::vector<std::string> Server::getPort() const
{
  return this->port;
}

std::string Server::getHost() const
{
  return this->host;
}

std::vector<std::string> Server::getServName() const
{
  return this->server_name;
}

void Server::check()
{
  if (!this->dup[0])
    throw std::string("Error: The host should be in the server.");

  if (this->port.empty())
    throw std::string("Error: it should be atlast one port or more in the server.");
}





