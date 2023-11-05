#include "reqParse.hpp"
#include <cstring>
#include <filesystem>
#include "sys/wait.h"

class Cgi {
private:
	Client		&request;
	char		**env;
public:
	Cgi( Client & );
	~Cgi();
	static bool fileExists( const char*, bool & );
	static bool hasReadPermission( const char* );
	char	*strDup( std::string );
	void	setEnv();
	void	executeCgi();
};