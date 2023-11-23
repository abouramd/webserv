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
                if (s_header(client, client.fd, "403 Forbidden", "text/html"))
                    return;
                client.is->open(get_page(client, 403).c_str());
            }
        }
    }
}



