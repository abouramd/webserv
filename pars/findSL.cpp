#include "Config.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include <map>
#include <string>
#include <utility>
#include <vector>

std::map<std::string, Location>::iterator   findLoca(Server& serv, const std::string &url)
{
  std::map<std::string, Location>::iterator it = serv.location.end();
  while (it-- != serv.location.begin())
  {
    if (!url.compare(0, it->first.length(), it->first))
    {
      return it;
    }
  }
  throw 404;
}

void   findServ(Client &client, std::vector<Server>& serv, const std::string &host, const std::string &url)
{
  std::string hostname = host;
    if (host.find(':') != std::string::npos)
        hostname = host.substr(0, host.find(':'));
    std::cout << hostname << std::endl;

  for (std::vector<Server>::iterator it = serv.begin(); it != serv.end(); it++)
  {
    for (std::vector<std::string>::iterator it_sn = it->server_name.begin(); it_sn != it->server_name.end(); it_sn++)
    {
      if (host == *it_sn)
      {
        client.maxBodySize = it->max_body_size;
        client.error_page = it->error_page;
        client.error_page_dfl = it->error_page_dfl;
        std::map<std::string, Location>::iterator t = findLoca(*it, url);
        client.location.first = t->first;
        client.location.second = t->second;
        return;
      }
    }
  }
  client.maxBodySize = serv.begin()->max_body_size;
  client.error_page = serv.begin()->error_page;
  client.error_page_dfl = serv.begin()->error_page_dfl;
  std::map<std::string, Location>::iterator t =  findLoca(*serv.begin(), url);
  client.location.first = t->first;
  client.location.second = t->second;
}
