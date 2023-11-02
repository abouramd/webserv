#include "responses.hpp"

void get(Client &client, std::string &get_query)
{
    if (is_dir(client.target) == 0)
    {
        std::cout << get_query << std::endl;
        std::string type = FileType::getContentType(get_ex(client.target));
        if (!is_cgi(client))
        {
            s_header(client.fd, "200 OK", type);
            client.is->open(client.target);
        }
        else
        {
            std::cout<< "Is cgi" << std::endl;
            client.state = CLOSE;
        }
    }
    else if (is_dir(client.target) == 1){
        std::cout<< "Is dir" << std::endl;
        client.state = CLOSE;
    }
    else
    {
        s_header(client.fd, "404 Page Not Found", "text/html");
        client.is->open("error_pages/404.html");
    }
}