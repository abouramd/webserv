#pragma once
#include "reqParse.hpp"

class Tools {
public:
	static bool	pathExists( const char*, bool & );
	static bool	hasReadPermission( const char* );
	static void	getAndCheckPath(std::string &, std::string &);
	static void	decode(std::string &);
	static void	decodeUri(std::string &);
};
