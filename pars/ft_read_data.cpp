#include <iostream>
#include <fstream>

std::string remove_spc(std::string &buff)
{
    std::string res;
    std::string::iterator ptr = buff.begin();

    while (ptr != buff.end() && (*ptr == ' ' || *ptr == '\t'))
      ptr++;
    while (ptr != buff.end())
    {
      if (*ptr == '#')
        break;
      if(!res.empty())
        res += ' ';
      while (ptr != buff.end() && *ptr != ' ' && *ptr != '\t')
      {
        res += *ptr;
        ptr++;
      }
      while (ptr != buff.end() && (*ptr == ' ' || *ptr == '\t'))
        ptr++;
    }
    return res;
}

int  ft_read(std::ifstream &file, std::string &buff)
{
    buff.clear();
    while (buff.empty())
    {
        std::getline(file, buff);
        if (buff.empty() && file.eof())
          return 0;
        buff = remove_spc(buff);
    }
    return buff.size();
}

