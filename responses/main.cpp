#include "responses.hpp"

void responses(Client &client)
{
    (void)client;
    std::cout << "Done" << std::endl;
    s_header(client.fd, "200 OK", "text/html");
    s_chank(client.fd, "Hello", 5);
    s_chank(client.fd, "", 0);
}