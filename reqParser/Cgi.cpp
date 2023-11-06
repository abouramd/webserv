#include "Cgi.hpp"

Cgi::Cgi(Client &req) : request(req) {}

Cgi::~Cgi() {
	for (int i = 0; env[i] != NULL; i++)
		delete[] env[i];
	delete[] env;
}

char	*Cgi::strDup(std::string src) {
	size_t	size = src.size();
	char	*newStr;

	newStr = new char[size + 1];
	std::strcpy(newStr, src.c_str());
	newStr[size] = 0;
	return newStr;
}

void Cgi::setEnv() {
	std::string	root(request.location->second.root.begin(), request.location->second.root.end() - 1);

	env = new char * [ENV_SIZE];
	env[0] = strDup("REQUEST_METHOD=" + request.method);
	env[1] = strDup("CONTENT_TYPE=" + request.headers["Content-Type"]);
	env[2] = strDup("CONTENT_LENGTH=" + request.headers["Content-length"]);
	env[3] = strDup("HTTP_USER_AGENT=" + request.headers["User-Agent"]);
	env[4] = strDup("SCRIPT_FILENAME=" + root + request.target);
	env[5] = strDup("SCRIPT_NAME=" + request.target);
	env[6] = strDup("REDIRECT_STATUS=200");
	env[7] = strDup("PATH_INFO=" + request.location->second.root + request.target);
	env[8] = NULL;
}

void  Cgi::executeCgi() {
	int           pid;
	char          *args[3];
	FILE          *in;
	FILE          *out;
	std::stringstream	ss;

	setEnv();
	ss << "temp/" << rand() << "_cgi_out.tmp";
	args[0] = new char[request.cgiScript.size() + 1];
	args[1] = new char[request.fullPath.size() + 1];
	bzero(args[0], request.cgiScript.size() + 1);
	bzero(args[1], request.fullPath.size() + 1);
	strcpy(args[0], request.cgiScript.c_str());
	strcpy(args[1], request.fullPath.c_str());
	args[2] = NULL;
	request.outfile->close();
	pid = fork();
	if (!pid) {
		std::cerr << args[0] << ", " << args[1] << std::endl;
		in = freopen(request.cgiFileName.c_str(), "r", stdin);
		out = freopen(std::string(ss.str()).c_str(), "w", stdout);
		(void)in;
		(void)out;
		execve(args[0], args, env);
		std::cerr << "execve fails!" << std::endl;
		exit (404);
	}
	else {
		waitpid(pid, NULL, 0);
		request.cgiFileName = ss.str();
	}
}