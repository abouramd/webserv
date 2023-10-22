#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <vector>
#include "Server.hpp"
#include "Socket.hpp"


class Config {
  private:
    std::string filename;
    std::ifstream file;
    std::vector<Server> servers;
    void init_data(int ac, char **av);
    void read_data();
    void creat_socket();
    void add_socket(sockaddr_in &addr, Server& sev, int& port);

  public:
    std::vector<Socket> socket;
    Config();
    // Config(const Config& obj);
    // Config& operator=(const Config& obj);
    ~Config();
    
    void pars(int ac, char **av); 
    std::vector<Socket>& get_socket();
};


#endif
