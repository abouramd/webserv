#include "responses.hpp"


std::string get_page(Client &client, int n)
{
    client.method = "GET";
    if (!is_dir(client.error_page[n]))
        return (client.error_page[n]);
    return (client.error_page_dfl[n]);
}

void error_handling(Client &client)
{
    if (client.statusCode == 403)
    {
        client.fullPath = get_page(client, 403);
        client.state_string = "403 Forbidden";
        return;
    }
    if (client.statusCode == 404)
    {
        client.fullPath = get_page(client, 404);
        client.state_string = "404 Not Found";
        return;
    }
    if (client.statusCode == 501)
    {
        client.fullPath = get_page(client, 501);
        client.state_string = "501 Not Implemented";
        return;
    }
    if (client.statusCode == 413)
    {
        client.fullPath = get_page(client, 413);
        client.state_string = "413 Request Entity Too Large";
        return;
    }
    if (client.statusCode == 405)
    {
        client.fullPath = get_page(client, 405);
        client.state_string = "405  Method Not Allowed";
        return;
    }
    if (client.statusCode == 400)
    {
        client.fullPath = get_page(client, 400);
        client.state_string = "400  Bad Request";
        return;
    }
    if (client.statusCode == 505)
    {
        client.fullPath = get_page(client, 505);
        client.state_string = "505 HTTP Version Not Supported";
        return;
    }
    if (client.statusCode == 415)
    {
        client.fullPath = get_page(client, 415);
        client.state_string = "415 Unsupported Media Type";
        return;
    }
    if (client.statusCode == 201)
    {
        client.fullPath = get_page(client, 201);
        client.state_string = "201 created";
        return;
    }
}
