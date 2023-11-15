#pragma once
#include "reqParse.hpp"

class Tools {
public:
	static          bool	pathExists( const char*, bool &, bool & r, bool & w);
	static          void	getAndCheckPath(std::string &, std::string &);
	static          void	decode(std::string &);
	static          void	decodeUri(std::string &);
	static          bool	getExtension(std::string & target, std::string & extension);
    static          std::string  toLower(std::string key);
    static size_t   findBin(const char *buf, char element, size_t start, size_t size);
};
