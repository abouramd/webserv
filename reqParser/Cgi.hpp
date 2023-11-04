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
	static bool fileExists(const char* filename);
	static bool hasReadPermission(const char* filename);
	char	*strDup(std::string src);
	void	setEnv();
	void	executeCgi();
};