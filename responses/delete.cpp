#include "responses.hpp"

int delete_dir(std::string name, int *n)
{
    DIR* dir;
    std::string path;

    dir = opendir(name.c_str());
    struct dirent* entry;
    while ((entry = readdir(dir)))
    {
        if (!std::string(entry->d_name).compare("..") || !std::string(entry->d_name).compare("."))
            continue;
        path = name +"/"+ std::string(entry->d_name) ;
        if (is_dir(path) == 1)
        {
            delete_dir(path, n);
        }
        else if (is_dir(path) == 0)
        {
            if (!access(path.c_str(), R_OK) && *n)
                remove(path.c_str());
            else
                *n = 0;
        }
        
    }
    remove(name.c_str());
    return 0;
}

void ft_delete(Client &client)
{
    int n = 1;

    if (is_dir(client.fullPath) == 0)
    {
        if (!access(client.fullPath.c_str(), R_OK))
            remove(client.fullPath.c_str());
        else
        {
            client.statusCode = 403;
            return;
        }
    }
    else if (is_dir(client.fullPath) == 1)
        delete_dir(client.fullPath, &n);
    else
    {
        if (s_header(client, client.fd, "404 Page Not Found", "text/html"))
        {
            client.fullPath = get_page(client, 404);
            client.state_string = "404 Page Not Found";
            client.method = "GET";
        }
        client.is->open(get_page(client, 404));
    }
    if (n)
    {
        if (s_header(client, client.fd, "204 Deleted", "text/html"))
        {
            client.fullPath = get_page(client, 204);
            client.state_string = "204 Deleted";
            client.method = "GET";
        }
        client.is->open(get_page(client, 204));
    }
    client.statusCode = 403;
}
