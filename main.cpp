#include "Client.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "Socket.hpp"
#include "reqParser/reqParse.hpp"
#include "responses/responses.hpp"
// #include <bits/types/struct_timeval.h>
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



/* start test multiplix */

std::string get_time() {
    // Get the current time in GMT
    std::time_t currentTime = std::time(NULL);

    // Convert the time to a struct tm (timeinfo) in GMT
    struct std::tm* timeinfo = std::gmtime(&currentTime);

    // Format the time as a string
    char buffer[200];
    strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %H:%M:%S GMT", timeinfo);

    return std::string(buffer);
}

void ft_send_header(int client_socket, std::string status, std::string type)
{
	std::string head = "HTTP/1.1 " + status + "\r\n";
	head += get_time() + "\r\n";
	head += "Server: webserver (abouramd)\r\n";
	head += "Content-Type: " + type + "\r\n";
	head += "Transfer-Encoding: chunked\r\n";	
    head += "\r\n";
	// std::cout << head << std::endl;
	send(client_socket, (char *)head.c_str(), head.size(), 0);
}

void base_trans(std::string& str, int n, const int &base)
{
  if (n >= base)
    base_trans(str, n / base, base);
  str += "0123456789abcdef"[n % base];
}

void send_chank(int fd, const char *content, const int size)
{
	std::string count;
	base_trans(count, size, 16);
	count += "\r\n";
  unsigned int new_size = count.size() + size + 2;
  char s[new_size];
  std::memcpy(s, count.c_str(), count.size());
  std::memcpy(s + count.size(), content, size);
  std::memcpy(s + new_size - 2, "\r\n", 2);
	send(fd, s, new_size, 0);
}

/* end test multiplix */

int max_fd( Config &obj )
{
  int max_fd = 0;
  std::time_t ttime = std::time(NULL);

  // int cn = 0;
  FD_ZERO(&obj.write_fd);
  FD_ZERO(&obj.read_fd);
  obj.timeout.tv_sec = 1;
  obj.timeout.tv_usec = 0;
  for (std::vector<Socket>::iterator it_s = obj.socket.begin(); it_s != obj.socket.end(); it_s++)
  {
    // std::cout << it_s->client.size() << std::endl;
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
      // cn++;
    }
    if (!FD_ISSET(it_s->getFd(), &obj.err_fd))
    {
      FD_SET(it_s->getFd(), &obj.read_fd);
      max_fd = it_s->getFd() < max_fd ? max_fd : it_s->getFd() + 1;
    }
    else
         std::cout << "error in fd socket." << std::endl;
  }
  // std::cout << "number of client is " << cn << std::endl;
  FD_ZERO(&obj.err_fd);
  return max_fd;
}

// int get_max_fd( std::vector<Socket> &my_s )
// {
//   int max_fd = -1;
//   // int cn = 0;

//   for (std::vector<Socket>::iterator it_s = my_s.begin(); it_s != my_s.end(); it_s++)
//   {
//     for (std::vector<Client>::iterator it_c = it_s->client.begin(); it_c != it_s->client.end(); it_c++)
//     {
//       max_fd = it_c->fd < max_fd ? max_fd : it_c->fd + 1;
//       // cn++;
//     }
//     max_fd = it_s->getFd() < max_fd ? max_fd : it_s->getFd() + 1;
//   }
//   // std::cout << "number of client is " << cn << std::endl;
//   return max_fd;
// }

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
    // std::cout << "hello" << std::endl;
    for (std::vector<Socket>::iterator it_s = obj.socket.begin(); it_s != obj.socket.end(); it_s++)
    {
      for (int i = it_s->client.size() - 1; i >= 0; i--)
      {
        if (FD_ISSET(it_s->client[i].fd, &obj.write_fd))
        {
          it_s->client[i].request_time = std::time(NULL);
          responses(it_s->client[i]);          
        }
        else if (FD_ISSET(it_s->client[i].fd, &obj.read_fd))
        {
          it_s->client[i].request_time = std::time(NULL);
          reqParser(it_s->client[i], it_s->client[i].fd, it_s->serv);
          if ( it_s->client[i].state == DONE )
            std::cout << BLUE << get_time() << " end of request and swap " << it_s->client[i].fd << " to responce." << DFL << std::endl;
        }
      }
      if (FD_ISSET(it_s->getFd(), &obj.read_fd))
      {
        if (obj.add_client(*it_s) != -1)
          std::cout << GREEN << get_time() << " new client fd " << it_s->client.back().fd << DFL << std::endl;
      }
    }
  }

  return 0;
}
