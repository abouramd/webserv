#include "responses.hpp"
#include <string>


char **get_env(Client &client)
{
    client.setEnv();
    std::map<std::string, std::string>  env = client.env;
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
    client.outfile->open(filename.c_str());
    client.outfile->close();
    if (!(client.pid = fork()))
    {
        char** env = get_env(client);
        int i = 0;
        while (env[i])
        {
            std::cout << env[i] << std::endl;
            i++;
        }
		    freopen(filename.c_str(), "w", stdout);
        char* argv[] = { const_cast<char*>(client.location.second.cgi.second[get_ex(client.fullPath)].c_str()), const_cast<char*>(client.fullPath.c_str()), NULL };
        execve(argv[0], argv, env);
        exit(244);
    }
    else
    {
        client.currentTime = time(NULL);
        client.is_cgi = 5;
        client.cgiFileName = filename;
        client.is->open(filename.c_str());
    }
}
