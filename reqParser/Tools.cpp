#include "Tools.hpp"
#include <ios>
#include <sys/stat.h>
#include <unistd.h>

bool	Tools::pathExists(const char* filename, bool & isDir, bool & r, bool & w) {
	struct stat fileState;

	std::cout << "path >> " << filename << ", " << isDir << std::endl;
	if (stat(filename, &fileState) == 0) {
		if (S_ISDIR(fileState.st_mode))
			isDir = true;	
    if (!access(filename, R_OK))
      r = true;
    if (!access(filename, W_OK))
      w = true;
		return true;
	}
	return false;
}

void	Tools::getAndCheckPath(std::string & uploadPath, std::string & extension) {
	std::stringstream	ss;
	bool				isDir(false), r(false), w(false);

	if (!Tools::pathExists(uploadPath.c_str(), isDir, r, w))
  {
    std::cout << "hello2" << std::endl;
    throw 404;
  }
  if (!isDir || !w)
		throw 403;
	ss << uploadPath + "/" << rand() << "_file." << extension;
	uploadPath = ss.str();
}

void	Tools::decode(std::string &str) {
	std::string			hex("0123456789ABCDEF");
	std::stringstream	unHex;

	if (str.size() != 2)
		throw 400;
	if (hex.find(str[0]) == std::string::npos || hex.find(str[1]) == std::string::npos)
		throw 400;
	unHex << str;
  str.clear();
  int tmp;
  unHex >> std::hex >> tmp;
  str = std::string(1, tmp);
  std::cout <<  "|" + str + "|" << " : " << tmp << std::endl;
}

void Tools::decodeUri(std::string &uri) {
	std::string	unHex;
	std::string	result;

	for (size_t i = 0; i < uri.size(); i++) {
		if (uri[i] == '%') {
			if (i + 2 >= uri.size())
				throw 400;
			unHex = uri.substr(i + 1, 2);
			Tools::decode(unHex);
			result += unHex;
			i += 2;
		}
		else
			result += uri[i];
	}
	uri = result;
  std::cout << "URL: " << uri << std::endl;
}

bool	Tools::getExtension(std::string & target, std::string & extension) {
	for (int i = target.size() - 1; i >= 0; i--) {
		if (target[i] == '.') {
			extension = target.substr(i + 1);
			return true;
		}
	}
	return true;
}

std::string Tools::toLower(std::string key) {
    for (size_t i = 0; i < key.size(); i++)
        key[i] = std::tolower(key[i]);
    return key;
}

size_t      Tools::findBin(const char *buf, char element, size_t start, size_t size) {
    while (start < size) {
        if (buf[start] == element)
            return start;
        start++;
    }
    return std::string::npos;
}

