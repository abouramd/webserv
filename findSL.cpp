#include "Config.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include <map>
#include <string>
#include <utility>
#include <vector>

std::map<std::string, Location>::iterator findLoca(Server& serv, const std::string &url)
{
  std::map<std::string, Location>::iterator it = serv.location.end();
  while (it-- != serv.location.begin())
  {
    if (!url.compare(0, it->first.length(), it->first))
      return it;
  }
  std::cout << BLUE << "didn't found the location" << DFL << std::endl;
  throw 403;
}

std::map<std::string, Location>::iterator findServ(unsigned long &max_body_size, std::vector<Server>& serv, const std::string &host, const std::string &url)
{

  for (std::vector<Server>::iterator it = serv.begin(); it != serv.end(); it++)
  {
    for (std::vector<std::string>::iterator it_sn = it->server_name.begin(); it_sn != it->server_name.end(); it_sn++)
    {
      if (host == *it_sn)
      {
        std::cout << BLUE << "found the server" << DFL << std::endl;
        max_body_size = it->max_body_size;
        return findLoca(*it, url);
      }
    }
  }
  std::cout << BLUE << "didn't found the server" << DFL << std::endl;
  
  max_body_size = serv.begin()->max_body_size;
  return findLoca(*serv.begin(), url);
}




