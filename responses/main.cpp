#include "responses.hpp"

void responses(Client &client)
{
    char buffer[100];
    std::string get_query = "";
    if (!client.is->is_open())
    {
        get_target(client, get_query);
        if (client.method == "GET")
            get(client);
    }
    else
    {
        client.is->read(buffer, sizeof(buffer));
        if (client.is->gcount())
            s_chank(client.fd, buffer, client.is->gcount());
        else
        {
            s_chank(client.fd, "", 0);
            client.is->close();
            client.state = CLOSE;
        }
    }
}