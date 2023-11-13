#include "responses.hpp"
#include <string>


char **get_env(Client &client)
{
    std::map<std::string, std::string>  env;
    // env["AUTH_TYPE"] = "";
    // env["CONTENT_LENGTH"] = ?;
    // env["CONTENT_TYPE"] = ?;
    //env["PATH_INFO"] = ?;
    //env["PATH_TRANSLATED"] = ?;
    env["QUERY_STRING"] = client.query;
    // env["REMOTE_HOST"] = ?;
    // env["REMOTE_USER"] = "";
    // env["REMOTE_IDENT"] = "";
    env["REQUEST_METHOD"] = client.method;
    // env["GATEWAY_INTERFACE"] = "CGI/1.1";
    // env["REQUEST_URI"] = ?;
    // env["SCRIPT_NAME"] = ?;
    env["SCRIPT_FILENAME"] = client.fullPath;
    // env["SERVER_NAME"] = ?;
    env["SERVER_PROTOCOL"] = "HTTP/1.1";
    // env["SERVER_PORT"] = ?;
    // env["SERVER_SOFTWARE"] = "webserv/1.0";
    env["REDIRECT_STATUS"] = "200";
    // env["REQUEST_SCHEME"] = "http";
    env["HTTPS"] = "off";
    // env["SERVER_SIGNATURE"] = "webserv/1.0";
    env["HTTP_COOKIE"] = client.headers["cookie"];

    char** keyValueArray = new char*[env.size() + 1];

    int i = 0;
    for (std::map<std::string, std::string>::const_iterator it = env.begin(); it != env.end(); ++it)
    {
        std::string keyValue = it->first + "=" + it->second;
        keyValueArray[i] = new char[keyValue.length() + 1];
        std::strcpy(keyValueArray[i], keyValue.c_str());
        i++;
    }
    keyValueArray[env.size()] = NULL;
    return(keyValueArray);
}

void cgi(Client &client)
{
    srand((unsigned) time(NULL));
    int num = rand();
    std::ostringstream convert;
    convert << num;
    std::string filename = convert.str();
    filename = "temp/" + filename;
    int fd = open(filename.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (!(client.pid = fork()))
    {
        char** env = get_env(client);
        int i = 0;
        while (env[i])
        {
            std::cout << env[i] << std::endl;
            i++;
        }
        dup2(fd, 1);
        char* argv[] = { const_cast<char*>(client.location.second.cgi.second[get_ex(client.fullPath)].c_str()), const_cast<char*>(client.fullPath.c_str()), NULL };
        execve(argv[0], argv, env);
    }
    else
    {
        client.currentTime = time(NULL);
        client.is_cgi = 5;
        client.cgiFileName = filename;
        client.is->open(filename.c_str());
    }
}
