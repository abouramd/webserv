#include "responses.hpp"

void error_handling(Client &client)
{
    if (client.statusCode == 405)
    {
        client.target = "error_pages/405.html";
        client.statestring = "405  Method Not Allowed";
        client.method = "GET";
        return;
    }
    if (client.statusCode == 400)
    {
        client.target = "error_pages/400.html";
        client.statestring = "400  Bad Request";
        client.method = "GET";
        return;
    }
    if (client.statusCode == 505)
    {
        client.target = "error_pages/505.html";
        client.statestring = "505  HTTP Version Not Supported";
        client.method = "GET";
        return;
    }
}