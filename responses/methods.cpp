#include "responses.hpp"

void get(Client &client)
{
    if (is_dir(client.target) == 0)
    {
        std::string type = FileType::getContentType(get_ex(client.target));
        s_header(client.fd, "200 OK", type);
        client.is->open(client.target);
    }
    else if (is_dir(client.target) == 1){
        std::cout<< "Is dir" << std::endl;
    }
    else
    {
        s_header(client.fd, "404 Page Not Found", "text/html");
        client.is->open("error_pages/404.html");
    }
}