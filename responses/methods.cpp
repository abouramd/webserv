#include "responses.hpp"

void get(Client &client)
{
    if (!access(client.fullPath.c_str(), F_OK) && access(client.fullPath.c_str(), R_OK))
    {
        client.fullPath = "error_pages/403.html";
        client.state_string = "403 Forbidden";
    }

    if (is_dir(client.fullPath) == 0)
    {
        std::string type = FileType::getContentType(get_ex(client.fullPath));
        if (!is_cgi(client))
        {
            s_header(client.fd, client.state_string, type);
            client.is->open(client.fullPath.c_str());
        }
        else
        {
            cgi(client);
        }
    }
    else if (is_dir(client.fullPath) == 1)
    {
        if (client.path[client.path.length() - 1] != '/' && client.opened != 5)
        {
            redirect(client, client.path + "/");
            return;
        }
        if (get_index(client))
        {
            std::string type = FileType::getContentType(get_ex(client.fullPath));
            if (!is_cgi(client))
            {
                s_header(client.fd, client.state_string, type);
                client.is->open(client.fullPath.c_str());
            }
            else
            {
                cgi(client);
            }
        }else{
            if (client.location->second.auto_index)
            {
                auto_index(client);
            }
            else
            {
                s_header(client.fd, "403 Forbidden", "text/html");
                client.is->open("error_pages/403.html");
            }
        }
    }
}

int delete_dir(std::string name)
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
            delete_dir(path);
        }
        else if (is_dir(path) == 0)
        {
            remove(path.c_str());
        }
        
    }
    remove(name.c_str());
    return 0;
}

void ft_delete(Client &client)
{
    if (is_dir(client.fullPath) == 0)
    {
        remove(client.fullPath.c_str());
        s_header(client.fd, "204 Deleted", "text/html");
        client.is->open("error_pages/204.html");
    }
    else if (is_dir(client.fullPath) == 1)
    {
        delete_dir(client.fullPath);
        s_header(client.fd, "204 Deleted", "text/html");
        client.is->open("error_pages/204.html");
    }
    else
    {
        s_header(client.fd, "404 Page Not Found", "text/html");
        client.is->open("error_pages/404.html");
    }
}
