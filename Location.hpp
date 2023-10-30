#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>

#define BLACK "\033[1;3;30m"
#define RED "\033[1;3;31m"
#define GREEN "\033[1;3;32m"
#define YELLOW "\033[1;3;33m"
#define BLUE "\033[1;3;34m"
#define PURPLE "\033[1;3;35m"
#define CYAN "\033[1;3;36m"
#define WHITE "\033[1;3;37m"
#define DFL "\033[0m"

class Location 
{
  private:
    bool dup[8];

    std::string key[14];
    void (Location::*ptr[14])(const std::string &);

  public:

    std::string root;
    std::string redirect; 
    std::vector<std::string> index;
    std::vector<std::string> allow_method;
    bool auto_index;
    std::pair<bool, std::string> uplode;
    std::pair<bool, std::map<std::string, std::string> > cgi;
    


    Location();
    // Location(const std::string name);
    // Location(const Location& obj);
    // Location& operator=(const Location& obj);
    ~Location();

    void init_data(std::ifstream &file);

    void find_key(const std::string &str);
    
    void set_root(const std::string &str);
    void set_auto_index(const std::string &str);
    void set_method(const std::string &str);
    void set_index(const std::string &str);
    void set_redirect(const std::string &str);
    void set_uplode(const std::string &str);
    void set_cgi(const std::string &str);
    void set_cgi_path(const std::string &str);
    
    void check();
};

int  ft_read(std::ifstream &file, std::string &buff);
std::vector<std::string> my_split(const std::string str);

#endif
