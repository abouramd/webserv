#include "reqParse.hpp"
#include <cstring>
#include "sys/wait.h"

class Cgi {
private:
	Client		&request;
	char		**env;
public:
	Cgi( Client & );
	~Cgi();
	char	*strDup(std::string src);
	void	setEnv();
	void	executeCgi();
};