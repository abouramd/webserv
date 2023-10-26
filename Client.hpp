#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <fstream>

class Client {
  public:
    int fd;
    std::ifstream& is;
    std::ofstream& os;
    Client(int fd, std::ifstream* i, std::ofstream* o);
    Client(const Client &obj);
    // Client& operator=(const Client &obj);
    // Client();
};

#endif // !CLIENT_HPP
