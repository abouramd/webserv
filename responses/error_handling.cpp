#include "responses.hpp"

void error_handling(Client &client)
{
    if (client.statusCode == 405)
    {
        client.target = "error_pages/405.html";
        client.state_string = "405  Method Not Allowed";
        client.method = "GET";
        return;
    }
    if (client.statusCode == 400)
    {
        client.target = "error_pages/400.html";
        client.state_string = "400  Bad Request";
        client.method = "GET";
        return;
    }
    if (client.statusCode == 505)
    {
        client.target = "error_pages/505.html";
        client.state_string = "505  HTTP Version Not Supported";
        client.method = "GET";
        return;
    }
    if (client.target.size() > 2048)
    {
        client.target = "error_pages/414.html";
        client.state_string = "414 Request-URI Too Long";
        client.method = "GET";
        return;
    }
}