#include "responses.hpp"
#include <string>


char **get_env(Client &client, std::string &get_query)
{
    std::map<std::string, std::string>  env;
    env["AUTH_TYPE"] = "";
    env["CONTENT_LENGTH"] = client.headers["Content-Length"];
    env["CONTENT_TYPE"] = client.headers["Content-Type"];
    //env["PATH_INFO"] = _client.req.get_path();
    //env["PATH_TRANSLATED"] = _client.req.real_uri;
    env["QUERY_STRING"] = get_query;
    env["REMOTE_HOST"] = client.headers["Host"];
    env["REMOTE_USER"] = "";
    env["HTTP_COOKIE"] = client.headers["Cookie"];;
    env["REMOTE_IDENT"] = "";
    env["REQUEST_METHOD"] = client.method;
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    // env["REQUEST_URI"] = _client.req.uri;
    // env["SCRIPT_NAME"] = _client.req.current_location.cgi_path;
    // env["SCRIPT_FILENAME"] = _client.req.current_location.root + "/" + _client.req.current_location.cgi_path;
    // env["SERVER_NAME"] = _client.server_ptr->name;
    env["SERVER_PROTOCOL"] = "HTTP/1.1";
    // env["SERVER_PORT"] = std::to_string(_client.server_ptr->port);
    env["SERVER_SOFTWARE"] = "webserv/1.0";
    env["REDIRECT_STATUS"] = "200";
    env["REQUEST_SCHEME"] = "http";
    env["HTTPS"] = "off";
    env["SERVER_SIGNATURE"] = "webserv/1.0";

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

void cgi(Client &client, std::string &get_query)
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
        char** env = get_env(client, get_query);
        int i = 0;
        while (env[i])
        {
            std::cout << env[i] << std::endl;
            i++;
        }
        
        std::cout << get_query << std::endl;
        std::string Query = "QUERY_STRING=" + get_query;
        dup2(fd, 1);
        char* argv[] = { const_cast<char*>(client.server->second.cgi.second["php"].c_str()), const_cast<char*>(client.target.c_str()), NULL };
        execve(argv[0], argv, env);
    }
    else
    {
        client.is_cgi = 5;
        client.method = filename;
        client.is->open(filename);
    }
}