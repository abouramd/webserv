#include "Client.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "Socket.hpp"
#include "reqParser/reqParse.hpp"
#include "responses/responses.hpp"
#include <cstring>
#include <sys/time.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <csignal>


std::string get_time() {
    std::time_t currentTime = std::time(NULL);
    struct std::tm* timeinfo = std::gmtime(&currentTime);
    char buffer[200];
    strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %H:%M:%S GMT", timeinfo);
    return std::string(buffer);
}

int max_fd( Config &obj )
{
  int max_fd = 0;
  std::time_t ttime = std::time(NULL);

  FD_ZERO(&obj.write_fd);
  FD_ZERO(&obj.read_fd);
  obj.timeout.tv_sec = 1;
  obj.timeout.tv_usec = 0;
  for (std::vector<Socket>::iterator it_s = obj.socket.begin(); it_s != obj.socket.end(); it_s++)
  {
    for (int i = it_s->client.size() - 1; i >= 0 ; i--)
    {
      if (!FD_ISSET(it_s->client[i].fd, &obj.err_fd) && it_s->client[i].state != CLOSE && it_s->client[i].request_time + 60 > ttime)
      {
        if (it_s->client[i].state == DONE)
          FD_SET(it_s->client[i].fd, &obj.write_fd);
        else
          FD_SET(it_s->client[i].fd, &obj.read_fd);
        max_fd = it_s->client[i].fd < max_fd ? max_fd : it_s->client[i].fd + 1;
      }
      else
      {
        std::cout << RED << get_time() << " remove client fd " << it_s->client[i].fd << DFL << std::endl;
        obj.rm_client(*it_s, i);
      }
    }
    if (!FD_ISSET(it_s->getFd(), &obj.err_fd))
    {
      FD_SET(it_s->getFd(), &obj.read_fd);
      max_fd = it_s->getFd() < max_fd ? max_fd : it_s->getFd() + 1;
    }
    else
         std::cout << "error in fd socket." << std::endl;
  }
  FD_ZERO(&obj.err_fd);
  return max_fd;
}

int main(int ac, char **av)
{
	std::signal(SIGPIPE, SIG_IGN);
	std::srand(time(NULL));
  Config obj;
  try{
    obj.pars(ac, av);
    FileType::set_mime_type("./mime.types");
  }
  catch ( const std::string err)
  {
    std::cerr << RED << err << DFL << std::endl;
    return 1;
  }

  while ( true )
  {
    if (select(max_fd(obj), &obj.read_fd, &obj.write_fd, &obj.err_fd, &obj.timeout) <= 0)
      continue;
    
    for (std::vector<Socket>::iterator it_s = obj.socket.begin(); it_s != obj.socket.end(); it_s++)
    {
      for (int i = it_s->client.size() - 1; i >= 0; i--)
      {
        if (FD_ISSET(it_s->client[i].fd, &obj.write_fd))
        {
          it_s->client[i].request_time = std::time(NULL);
          responses(it_s->client[i]);          
        }
        if (FD_ISSET(it_s->client[i].fd, &obj.read_fd))
        {
          it_s->client[i].request_time = std::time(NULL);
          reqParser(it_s->client[i], it_s->client[i].fd, it_s->serv);
          if ( it_s->client[i].state == DONE )
          {
            std::cout << it_s->client[i].method << " : " << it_s->client[i].target << " : " << it_s->client[i].version << std::endl;
            std::cout << "status: " << it_s->client[i].statusCode << std::endl;
            std::cout << BLUE << get_time() << " end of request and swap " << it_s->client[i].fd << " to responce." << DFL << std::endl;
          }
        }
      }
      if (FD_ISSET(it_s->getFd(), &obj.read_fd))
      {
        if (obj.add_client(*it_s) != -1)
          std::cout << GREEN << get_time() << " new client fd " << it_s->client.back().fd << DFL << std::endl;
      }
      if (FD_ISSET(it_s->getFd(), &obj.write_fd))
        continue;
    }
  }

  return 0;
}
