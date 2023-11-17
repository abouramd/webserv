#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"
#include "Socket.hpp"
#include <fstream>
#include <iostream>
#include <sys/socket.h>
#include <vector>

class Config {
private:
  std::string filename;
  std::ifstream file;
  std::vector<Server> servers;
  void init_data(int ac, char **av);
  void read_data();
  void creat_socket();
  void add_socket(sockaddr_in &addr, Server &sev, int &port);

public:
  std::vector<Socket> socket;
  std::map<int, std::pair<std::ifstream *, std::ofstream *> > map_files;
  fd_set read_fd, write_fd, err_fd;
  timeval timeout;

  Config();
  ~Config();


  int add_client(Socket& sock);

  void rm_client(Socket& sock, int index);

  void pars(int ac, char **av);
  std::vector<Socket> &get_socket();
};

#endif
