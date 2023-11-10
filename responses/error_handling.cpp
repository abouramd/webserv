#include "responses.hpp"


// 413 501 403
void error_handling(Client &client)
{
    if (client.statusCode == 403)
    {
        client.fullPath = "error_pages/403.html";
        client.state_string = "403 Forbidden";
        client.method = "GET";
        return;
    }
    if (client.statusCode == 404)
    {
        client.fullPath = "error_pages/404.html";
        client.state_string = "404 Not Found";
        client.method = "GET";
        return;
    }
    if (client.statusCode == 501)
    {
        client.fullPath = "error_pages/501.html";
        client.state_string = "501 Not Implemented";
        client.method = "GET";
        return;
    }
    if (client.statusCode == 413)
    {
        client.fullPath = "error_pages/413.html";
        client.state_string = "413 Request Entity Too Large";
        client.method = "GET";
        return;
    }
    if (client.statusCode == 405)
    {
        client.fullPath = "error_pages/405.html";
        client.state_string = "405  Method Not Allowed";
        client.method = "GET";
        return;
    }
    if (client.statusCode == 400)
    {
        client.fullPath = "error_pages/400.html";
        client.state_string = "400  Bad Request";
        client.method = "GET";
        return;
    }
    if (client.statusCode == 505)
    {
        client.fullPath = "error_pages/505.html";
        client.state_string = "505 HTTP Version Not Supported";
        client.method = "GET";
        return;
    }
    if (client.statusCode == 415)
    {
        client.fullPath = "error_pages/415.html";
        client.state_string = "415 Unsupported Media Type";
        client.method = "GET";
        return;
    }
    if (client.statusCode == 201)
    {
        client.fullPath = "error_pages/201.html";
        client.state_string = "201 created";
        client.method = "GET";
        return;
    }
}
