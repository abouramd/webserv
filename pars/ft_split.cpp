#include <iostream>
#include <vector>

std::vector<std::string> my_split(const std::string str, const std::string lim)
{
    std::vector<std::string> vec;
    std::string buff;
    size_t i = 0;
    size_t size = str.size();

    while (i < size && (lim.find(str[i]) != std::string::npos))
        i++;
    while (i < size)
    {
        while (i < size && (lim.find(str[i]) == std::string::npos))
        {
            buff += str[i];
            i++;
        }
        vec.push_back(buff);
        while (i < size && (lim.find(str[i]) != std::string::npos))
            i++;
        buff.clear();
    }
    return vec;
}

std::vector<std::string> my_split(const std::string str)
{
  return my_split(str, " \t");
}


