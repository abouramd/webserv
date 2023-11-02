#include "responses.hpp"

void responses(Client &client)
{
    get_target(client);
    if (is_dir(client.target) == 0)
    {
        std::cout << "Is file" << std::endl;
    }
    else if (is_dir(client.target) == 1){
        std::cout<< "Is dir" << std::endl;
    }
    else
    {
        std::cout << "Not found" << std::endl;
    }
    s_header(client.fd, "200 OK", "text/html");
    s_chank(client.fd, "Hello", 5);
    s_chank(client.fd, "", 0);
}