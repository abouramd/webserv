#include "responses.hpp"
#include <string>


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
        dup2(fd, 1);
        char* argv[] = { const_cast<char*>(client.server->second.cgi.second["php"].c_str()), const_cast<char*>(client.target.c_str()), NULL };
        char* envp[] = { NULL };
        execve(argv[0], argv, envp);
    }
    else
    {
        client.is_cgi = 5;
        client.method = filename;
        client.is->open(filename);
    }
}