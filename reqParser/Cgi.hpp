#include "reqParse.hpp"
#include <cstring>
#include <filesystem>
#include "sys/wait.h"
#define ENV_SIZE 8

class Cgi {
private:
	Client		&request;
	char		**env;
public:
	Cgi( Client & );
	~Cgi();
	static bool pathExists( const char*, bool & );
	static bool hasReadPermission( const char* );
	char	*strDup( std::string );
	void	setEnv();
	void	executeCgi();
};