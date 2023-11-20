#include "Tools.hpp"
#include <cstring>
#include <filesystem>
#include "sys/wait.h"
#define ENV_SIZE 9

class Cgi {
private:
	Client		&request;
	char		**env;
public:
	Cgi( Client & );
	~Cgi();
	char	*strDup( std::string );
	void	setEnv();
	void	executeCgi();
};
