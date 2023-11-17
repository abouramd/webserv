#include "Location.hpp"
#include <algorithm>
#include <fstream>
#include <cstring>

// check duplicate 


Location::Location() {
    this->auto_index = false;
    this->uplode.first = false;
    this->cgi.first = false;
    
    for (int i = 0;i < 8;i++)
      this->dup[i] = false;

    this->key[0] = "root";
    this->key[1] = "index";
    this->key[2] = "allow_method";
    this->key[3] = "auto_index";
    this->key[4] = "return";
    this->key[5] = "uploade";
    this->key[6] = "cgi";
    this->key[7] = "cgi_path";
    this->key[8] = "";
    this->key[9] = "";
    this->key[10] = "";

    ptr[0] = &Location::set_root;
    ptr[1] = &Location::set_index;
    ptr[2] = &Location::set_method;
    ptr[3] = &Location::set_auto_index;
    ptr[4] = &Location::set_redirect;
    ptr[5] = &Location::set_uplode;
    ptr[6] = &Location::set_cgi;
    ptr[7] = &Location::set_cgi_path;
    ptr[8] = NULL;
    ptr[9] = NULL;
    ptr[10] = NULL;
    ptr[11] = NULL;

}

// Location::Location(const Location &obj) {
//     *this = obj;
// }
//
// Location& Location::operator=(const Location &obj) {
//     this->index = obj.index;
//     this->root = obj.root;
//     this->allow_method = obj.allow_method;
//     this->redirect = obj.redirect;
//     this->auto_index = obj.auto_index;
//     this->uplode = obj.uplode;
//     return *this;
// }

Location::~Location() {
}

void Location::find_key(const std::string &str)
{
  std::vector<std::string> vec = my_split(str);
  size_t index = 0;

  while (!this->key[index].empty())
  {
    if (vec[0] == this->key[index])
    {
      (this->*ptr[index])(str);
      return;
    }
    index++;
  }
  throw std::string("Error: unknowing `" + str + "`.");
}

void Location::init_data(std::ifstream &file)
{
  std::string buffer;

  while (ft_read(file, buffer))
  {
    if (buffer == "}")
      return;
    this->find_key(buffer);
  }
  throw std::string("Error: unclosed bracket");
}

void Location::set_root(const std::string &str)
{
  struct stat sb;

  if (this->dup[0])
    throw std::string("Error: The root is duplicated `" + str + "`.");
  this->dup[0] = true;

  std::vector<std::string> vec = my_split(str);
  
  if (vec.size() != 2)
    throw std::string("Error: number of args (" + str + ")");
  
  if (!(stat(vec[1].c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)))
    throw std::string("Error: root Path `" + std::string(strerror(errno)) + "` (" + str + ")");

  this->root = vec[1];
}

void Location::set_auto_index(const std::string &str)
{
  if (this->dup[1])
    throw std::string("Error: The auto_index is duplicated `" + str + "`.");
  this->dup[1] = true;
  
  std::vector<std::string> vec = my_split(str);
  
  if (vec.size() != 2)
    throw std::string("Error: number of args (" + str + ")");
  
  if (vec[1] == "on")
    this->auto_index = true;
  else if (vec[1] == "off")
    this->auto_index = false;
  else
    throw std::string("Error: unknowing option `" + vec[1] + "` in (" + str + ")");
}

void Location::set_method(const std::string &str)
{
   std::vector<std::string> vec = my_split(str);
  
  if (vec.size() < 2)
    throw std::string("Error: number of args (" + str + ")");
  
  std::vector<std::string>::iterator it = vec.begin() + 1;
  
  while (it != vec.end())
  {
    if (*it == "GET")
    {
      if (this->dup[2])
        throw std::string("Error: The GET method is duplicated `" + str + "`.");
      this->dup[2] = true;

      this->allow_method.push_back("GET");
    }
    else if (*it == "POST")
    {
      if (this->dup[3])
        throw std::string("Error: The POST method is duplicated `" + str + "`.");
      this->dup[3] = true;
      
      this->allow_method.push_back("POST");
    }
    else if (*it == "DELETE")
    {
      if (this->dup[4])
        throw std::string("Error: The DELETE method is duplicated `" + str + "`.");
      this->dup[4] = true;
      
      this->allow_method.push_back("DELETE");
    }
    else
      throw std::string("Error: unknowing method `" + *it + "` in (" + str + ")");
    it++;
  }
}

void Location::set_index(const std::string &str)
{
  std::vector<std::string> vec = my_split(str);
  
  if (vec.size() < 2)
    throw std::string("Error: number of args (" + str + ")");
  
  std::vector<std::string>::iterator it = vec.begin() + 1;
  while (it != vec.end())
  {
    // ceck nginx if the same index can use multi time  
    
    this->index.push_back(*it);
    it++;
  }
}

void Location::set_redirect(const std::string &str)
{
  if (this->dup[5])
    throw std::string("Error: redirect is duplicated `" + str + "`.");
  this->dup[5] = true;

  std::vector<std::string> vec = my_split(str);
  
  if (vec.size() != 2)
    throw std::string("Error: number of args (" + str + ")");
  
  this->redirect = vec[1];
}

void Location::set_uplode(const std::string &str)
{
  // struct stat sb;
  
  if (this->dup[6])
    throw std::string("Error: uplode is duplicated `" + str + "`.");
  this->dup[6] = true;
 
  std::vector<std::string> vec = my_split(str);
  
  if (vec.size() != 3)
    throw std::string("Error: number of args (" + str + ")");
  
  if (vec[1] == "on")
    this->uplode.first = true;
  else if (vec[1] == "off")
    this->uplode.first = false;
  else
    throw std::string("Error: unknowing option `" + vec[1] + "` in (" + str + ")");

  if (vec[2][0] != '/')
    throw std::string("Error: uplode Path should start with `/` (" + str + ")");
  // if (!(stat(vec[1].c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)))
  //   throw std::string("Error: uplode Path (" + str + ")");
  
  this->uplode.second = vec[2];
}

void Location::set_cgi(const std::string &str)
{
  if (this->dup[7])
    throw std::string("Error: CGI is duplicated `" + str + "`.");
  this->dup[7] = true;
 
  std::vector<std::string> vec = my_split(str);
  
  if (vec.size() != 2)
    throw std::string("Error: number of args (" + str + ")");
  
  if (vec[1] == "on")
    this->cgi.first = true;
  else if (vec[1] == "off")
    this->cgi.first = false;
  else
    throw std::string("Error: unknowing option `" + vec[1] + "` in (" + str + ")");
}


void Location::set_cgi_path(const std::string &str)
{
  struct stat sb;
  
  std::vector<std::string> vec = my_split(str);
  
  if (vec.size() != 3)
    throw std::string("Error: number of args (" + str + ")");
  
  if (!(stat(vec[2].c_str(), &sb) == 0 && S_ISREG(sb.st_mode)))
    throw std::string("Error: CGI Path (" + str + ")");

  if (this->cgi.second.find(vec[1]) != this->cgi.second.end())
    throw std::string("Error: CGI `" + vec[1] + "` is duplicated (" + str + ")");

  if (sb.st_mode & S_IXUSR)
    this->cgi.second[vec[1]] = vec[2];
  else 
    throw std::string("Error: CGI Path `" + vec[2] + "` is not exec (" + str + ")");
}

void Location::check()
{
  if (this->dup[0] == false)
    throw std::string("Error: The root should be in the location.");
  if (this->allow_method.empty())
    this->allow_method.push_back("GET");
  
}

