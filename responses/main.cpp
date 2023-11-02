#include "responses.hpp"

void responses(Client &client)
{
    char buffer[100];
    if (!client.is->is_open())
    {
        get_target(client);
        if (is_dir(client.target) == 0)
        {
            std::string type = FileType::getContentType(get_ex(client.target));
            s_header(client.fd, "200 OK", type);
            client.is->open(client.target, std::ios::in | std::ios::binary);
        }
        else if (is_dir(client.target) == 1){
            std::cout<< "Is dir" << std::endl;
        }
        else
        {
            s_header(client.fd, "404 Page Not Found", "text/html");
            client.is->open("error_pages/404.html", std::ios::in | std::ios::binary);
        }
    }
    else
    {
        if (client.is->read(buffer, sizeof(buffer)))
            s_chank(client.fd, buffer, sizeof(buffer));
        else
            client.state = CLOSE;
    }
}