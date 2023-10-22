#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"
#include <fstream>
#include <string>

class Server
{
  private:
    bool dup[5];
   
    std::string key[14];
    void (Server::*ptr[14])(const std::string&);

    std::ifstream *file;

    long str_to_int(std::string &str, long m);

  public:

    std::string host;
    std::vector<std::string> port;
    std::vector<std::string> server_name;
    std::map<std::string, Location> location;
    size_t max_body_size;


    Server();
    // Server(const Server& obj);
    // Server& operator=(const Server& obj);
    ~Server();

    void init_data(std::ifstream &file);

    void set_host(const std::string &str);
    void set_port(const std::string &str);
    void set_server_name(const std::string &str);
    void set_location(const std::string &str);
    void set_max_body(const std::string &str);

    void find_key(const std::string &str);

    std::vector<std::string> getPort() const;
    std::vector<std::string> getServName() const;

    std::string getHost() const;

};

#endif
