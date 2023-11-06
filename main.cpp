#include "Client.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "Socket.hpp"
#include "reqParser/reqParse.hpp"
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
	send(fd, count.c_str(), count.size(), 0);
	send(fd, content, size, 0);
	send(fd, "\r\n", 2, 0);
}

/* end test multiplix */


int get_max_fd( std::vector<Socket> &my_s )
{
  int max_fd = -1;

  for (std::vector<Socket>::iterator it_s = my_s.begin(); it_s != my_s.end(); it_s++)
  {
    for (std::vector<Client>::iterator it_c = it_s->client.begin(); it_c != it_s->client.end(); it_c++)
      max_fd = it_c->fd < max_fd ? max_fd : it_c->fd + 1;
    max_fd = it_s->getFd() < max_fd ? max_fd : it_s->getFd() + 1;
  }
  // std::cout << "max fd is " << max_fd << std::endl;
  return max_fd;
}

int main(int ac, char **av)
{
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
  std::map<int, std::pair<std::ifstream*, std::ofstream*> > map_files;
  std::vector<Socket> &my_s = obj.get_socket();
  fd_set sread , swrite;
  FD_ZERO(&sread);
  FD_ZERO(&swrite);
  for (std::vector<Socket>::iterator it_s = my_s.begin(); it_s != my_s.end(); it_s++)
    FD_SET(it_s->getFd(), &sread);
  while (1)
  {
    fd_set tmp_read = sread, tmp_write = swrite;
    int ready = select(get_max_fd(my_s), &tmp_read, &tmp_write, NULL, NULL);
    // std::cout << "pass select" << std::endl;
    if (ready == -1)
    {
      std::cerr << "select failed" << std::endl;
      return 1;
    }

    for (std::vector<Socket>::iterator it_s = my_s.begin(); it_s != my_s.end(); it_s++)
    {
      for (int i = it_s->client.size() - 1; i >= 0; i--)
      {
        if (FD_ISSET(it_s->client[i].fd, &tmp_write))
        {
          std::ifstream in("index.html");
          std::string   file;

          std::getline(in, file, '\0');
          std::cout << GREEN << get_time() << " send responce to "  << it_s->client[i].fd << DFL << std::endl;
          ft_send_header(it_s->client[i].fd, "200 OK", "text/html");
          send_chank(it_s->client[i].fd, file.c_str(), file.size());
          send_chank(it_s->client[i].fd, "", 0);
          it_s->client[i].state = CLOSE; 
          
          if (it_s->client[i].state == CLOSE ) {
            FD_CLR(it_s->client[i].fd, &swrite);
            close(it_s->client[i].fd);
            delete map_files[it_s->client[i].fd].first;
            delete map_files[it_s->client[i].fd].second;
            map_files.erase(it_s->client[i].fd);
            std::cout << PURPLE << get_time() << " remove a client " << it_s->client[i].fd << DFL << std::endl;
            it_s->client.erase(it_s->client.begin() + i); 
          }
        }
        else if (FD_ISSET(it_s->client[i].fd, &tmp_read))
        {
          it_s->client[i].request_time = std::time(NULL);
          reqParser(it_s->client[i], it_s->client[i].fd, it_s->serv);
          if ( it_s->client[i].state == DONE )
          {
            FD_CLR(it_s->client[i].fd, &sread);
            FD_SET(it_s->client[i].fd, &swrite);
            std::cout << BLUE << get_time() << " end of request and swap " << it_s->client[i].fd << " to responce." << DFL << std::endl;
          }
          else if (it_s->client[i].state == CLOSE ) {
            FD_CLR(it_s->client[i].fd, &sread);
            close(it_s->client[i].fd);
            delete map_files[it_s->client[i].fd].first;
            delete map_files[it_s->client[i].fd].second;
            map_files.erase(it_s->client[i].fd);
            std::cout << PURPLE << get_time() << " remove a client " << it_s->client[i].fd << DFL << std::endl;
            it_s->client.erase(it_s->client.begin() + i); 
          }
        }
        else if (it_s->client[i].state != DONE && it_s->client[i].request_time + 5 < std::time(NULL)) {
          FD_CLR(it_s->client[i].fd, &sread);
          close(it_s->client[i].fd);
          delete map_files[it_s->client[i].fd].first;
          delete map_files[it_s->client[i].fd].second;
          map_files.erase(it_s->client[i].fd);
          std::cout << PURPLE << get_time() << " remove a client " << it_s->client[i].fd << DFL << std::endl;
          it_s->client.erase(it_s->client.begin() + i); 
        }

      }
      if (FD_ISSET(it_s->getFd(), &tmp_read))
      {
        int fd = accept(it_s->getFd(), NULL, NULL);
        map_files[fd] = make_pair(new std::ifstream, new std::ofstream);
        FD_SET(fd, &sread);
        it_s->client.push_back(Client(fd, map_files[fd].first, map_files[fd].second));
        std::cout << YELLOW << get_time() << " accept a client " << fd << DFL << std::endl;
      }
    }
  }

  return 0;
}
