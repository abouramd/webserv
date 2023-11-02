#ifndef Socket_hpp
#define Socket_hpp

#include "Client.hpp"
#include "Server.hpp"
#include <netinet/in.h>
#include <string>
#include <vector>

// class Client;
class Socket {
private:
  int Socket_fd;
  std::string host;
  int port;
  sockaddr_in server;

  // Socket(const Socket& obj);
  // Socket& operator=(const Socket& obj);

public:
  Socket();
  ~Socket();

  std::vector<std::string> server_name;
  
  // void setFd(const int fd);
  int getFd() const;

  void setHost(const std::string h);
  std::string getHost() const;

  in_addr getSinAddr() const;
  int getSinPort() const;

  void setPort(const int p);
  int getPort() const;

  void connectASocket();

  void check_server_name(std::vector<std::string> &ports);

  std::vector<Client> client;
  std::vector<Server> serv;
};

#endif
