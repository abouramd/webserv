#include "responses.hpp"

void responses(Client &client)
{
    char buffer[100];
    if (!client.is->is_open())
    {
        get_target(client);
        if (is_dir(client.target) == 0)
        {
            s_header(client.fd, "200 OK", "image/jpeg");
            client.is->open(client.target, std::ios::in | std::ios::binary);
        }
        else if (is_dir(client.target) == 1){
            std::cout<< "Is dir" << std::endl;
        }
        else
        {
            std::cout << "Not found" << std::endl;
        }
    }
    else
    {
        if (client.is->read(buffer, sizeof(buffer)) || client.is->gcount())
            s_chank(client.fd, buffer, sizeof(buffer));
        else
            client.state = CLOSE;
    }
}