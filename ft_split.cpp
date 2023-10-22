#include <iostream>
#include <vector>

std::vector<std::string> my_split(const std::string str)
{
    std::vector<std::string> vec;
    std::string buff;
    size_t i = 0;
    size_t size = str.size();

    while (i < size && (str[i] == ' ' || str[i] == '\t'))
        i++;
    while (i < size)
    {
        while (i < size && str[i] != ' ' && str[i] != '\t') {
            buff += str[i];
            i++;
        }
        vec.push_back(buff);
        while (i < size && (str[i] == ' ' || str[i] == '\t'))
            i++;
        buff.clear();
    }
    return vec;
}

// int main()
// {
//     std::vector<std::string> v = my_split("hello     listen 90");
//     for (const std::string &s:v)
//         std::cout << "|" + s + "|" << std::endl;
//     return 0;
// }
