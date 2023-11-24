#include "responses.hpp"

void get(Client &client)
{
    if (!access(client.fullPath.c_str(), F_OK) && access(client.fullPath.c_str(), R_OK))
    {
        client.statusCode = 403;
        return;
    }

    if (is_dir(client.fullPath) == 0)
    {
        std::string type = FileType::getContentType(get_ex(client.fullPath));
        if (!is_cgi(client))
        {
            if (s_header(client, client.fd, client.state_string, type))
                return;
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
                if(s_header(client, client.fd, client.state_string, type))
                    return;
                client.is->open(client.fullPath.c_str());
            }
            else
            {
                cgi(client);
            }
        }else{
            if (client.location.second.auto_index)
            {
                auto_index(client);
            }
            else
            { 
              client.statusCode = 403;
              return;
            }
        }
    }
}



