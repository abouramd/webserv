#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <fstream>

class Client {
  public:
    int fd;
    std::ifstream* s;
    Client(const Client&);
    Client& operator=(const Client &obj);
    Client();
};

#endif // !CLIENT_HPP
