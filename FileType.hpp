#ifndef FILETYPE_HPP
#define FILETYPE_HPP

#include <map>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

class FileType {
private:
  static std::map<std::string, std::string> fileTypes;
  static std::map<std::string, std::string> fileTypesRev;
public:
  static void set_mime_type(std::string filename);
  static std::string getExt(const std::string & contenttype);
  static std::string getContentType(const std::string & ext);
};

std::vector<std::string> my_split(const std::string str);
int ft_read(std::ifstream &file, std::string &buff);

#endif
