#include "FileType.hpp"

std::map<std::string, std::string> FileType::fileTypes;
std::map<std::string, std::string> FileType::fileTypesRev;

void FileType::set_mime_type(std::string filename) {
  std::string buff;
  std::ifstream file(filename.c_str());
  std::vector<std::string> vec;

  if (!file.is_open())
    throw std::string("Erorr: failed to open " + filename);
  while (ft_read(file, buff))
  {
    vec = my_split(buff);
    
    if (vec.size() < 2)
      throw std::string("Erorr: in this line " + buff);

    for (std::vector<std::string>::iterator key = vec.begin() + 1; key != vec.end(); key++)
    {
      if (fileTypes.find(*key) != fileTypes.end())
        throw std::string("Erorr: this key is dup..." + *key);
      fileTypes[*key] = vec[0];
    }
    if (fileTypesRev.find(vec[0]) != fileTypesRev.end())
      throw std::string("Erorr: this key is dup... " + vec[0]);
    fileTypesRev[vec[0]] = vec[1];
  }
  file.close();
}

std::string FileType::getExt(const std::string & contenttype)
{
  std::cout << contenttype << std::endl;
  for (std::map<std::string, std::string>::iterator it = fileTypesRev.begin(); it != fileTypesRev.end(); it++)
    if (it->first == contenttype)
      return it->second;
  throw 415;
}

std::string FileType::getContentType(const std::string & ext)
{
  for (std::map<std::string, std::string>::iterator it = fileTypes.begin(); it != fileTypes.end(); it++)
    if (it->first == ext)
      return it->second;
  return "application/octet-stream";
}

// int main()
// {
//   try {
//     FileType::set_mime_type("./mime.types");
//   } catch (std::string & ext) {
//     std::cout << ext << std::endl;
//     return 1;
//   }
//   std::cout << FileType::getContentType("html") << std::endl;
//   std::cout << FileType::getContentType("css") << std::endl;
//   std::cout << FileType::getContentType("mp4") << std::endl;
//   std::cout << FileType::getExt("video/mp4") << std::endl;
//   return 0;
// }
