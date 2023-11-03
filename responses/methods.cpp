#include "responses.hpp"

void get(Client &client, std::string &get_query, std::string &ftarget)
{
    (void)get_query;
    if (is_dir(client.target) == 0)
    {
        std::string type = FileType::getContentType(get_ex(client.target));
        if (!is_cgi(client))
        {
            s_header(client.fd, "200 OK", type);
            client.is->open(client.target);
        }
        else
        {
            cgi(client);
        }
    }
    else if (is_dir(client.target) == 1)
    {
        if (ftarget[ftarget.length() - 1] != '/')
            redirect(client, ftarget + "/");
        if (get_index(client))
        {
            std::string type = FileType::getContentType(get_ex(client.target));
            if (!is_cgi(client))
            {
                s_header(client.fd, "200 OK", type);
                client.is->open(client.target);
            }
            else
            {
                cgi(client);
            }
        }else{
            if (client.server->second.auto_index)
            {
                auto_index(client, ftarget);
            }
            else
            {
                s_header(client.fd, "403 Forbidden", "text/html");
                client.is->open("error_pages/403.html");
            }
        }
    }
    else
    {
        s_header(client.fd, "404 Page Not Found", "text/html");
        client.is->open("error_pages/404.html");
    }
}