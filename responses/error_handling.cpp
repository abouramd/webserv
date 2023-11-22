#include "responses.hpp"

std::string creat_rand_file(const std::string content, const std::string page, int first)
{
  std::srand(std::time(NULL));
  std::string name;
  std::stringstream ss;
  ss << first;
  ss << std::rand();
  ss << ".html";
  // std::cout << ss.str() << std::endl;
  name = ss.str();
  std::ofstream out(name.c_str());
  out << "<!DOCTYPE html><html><head><title> " + content + " </title>" << std::endl;
  out << "<style>body,p{padding:20px}ul{margin:0;padding:0}body{background-color:#f8f8f8;font-family:Arial,sans-serif;display:flex;flex-direction:column;align-items:center;justify-content:center;min-height:100vh}h1{background-color:#333;color: #f00;border:2px solid #f00;border-radius:10px;color:#f00;padding:10px;text-align:center;font-size:24px}p{background-color:#fff;border:2px solid #f00;border-radius:10px;box-shadow:0 0 15px rgba(0,0,0,.2);width:80%;list-style-type:none}</style>" << std::endl;
  out << "<link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.1/font/bootstrap-icons.css\"></head><body>" << std::endl;
  out << "<h1><i class=\"bi bi-exclamation-triangle-fill\"></i> " + content + " <i class=\"bi bi-exclamation-triangle-fill\"></i></h1>" << std::endl;
  out << "<p>the default of this error page have some problemes this is just a back up.<br>" << std::endl;
  out << "creat file in this path " + page + " to fix this</p>" << std::endl;
  out << "</body></html>" << std::endl;
  out.close();
  return name;
}

std::string get_page(Client &client, int n)
{
    client.method = "GET";
    if (!is_dir(client.error_page[n]) && !access(client.error_page[n].c_str(), R_OK) && n != 201)
        return (client.error_page[n]);
    if (!is_dir(client.error_page_dfl[n]) && !access(client.error_page_dfl[n].c_str(), R_OK))
        return (client.error_page_dfl[n]);
    client.tmp_error = creat_rand_file(client.state_string, client.error_page_dfl[n], n);
    return (client.tmp_error);
}

int error_handling(Client &client)
{
    if (client.statusCode == 403)
    {
        client.state_string = "403 Forbidden";
        client.fullPath = get_page(client, 403);
        return 1;
    }
    if (client.statusCode == 408)
    {
        client.state_string = "408 Request Timeout";
        client.fullPath = get_page(client, 408);
        return 1;
    }
    if (client.statusCode == 500)
    {
        client.state_string = "500 Internal Server Error";
        client.fullPath = get_page(client, 500);
        return 1;
    }
    if (client.statusCode == 404)
    {
        client.state_string = "404 Not Found";
        client.fullPath = get_page(client, 404);
        return 1;
    }
    if (client.statusCode == 501)
    {
        client.state_string = "501 Not Implemented";
        client.fullPath = get_page(client, 501);
        return 1;
    }
    if (client.statusCode == 413)
    {
        client.state_string = "413 Request Entity Too Large";
        client.fullPath = get_page(client, 413);
        return 1;
    }
    if (client.statusCode == 405)
    {
        client.state_string = "405  Method Not Allowed";
        client.fullPath = get_page(client, 405);
        return 1;
    }
    if (client.statusCode == 400)
    {
        client.state_string = "400  Bad Request";
        client.fullPath = get_page(client, 400);
        return 1;
    }
    if (client.statusCode == 505)
    {
        client.state_string = "505 HTTP Version Not Supported";
        client.fullPath = get_page(client, 505);
        return 1;
    }
    if (client.statusCode == 415)
    {
        client.state_string = "415 Unsupported Media Type";
        client.fullPath = get_page(client, 415);
        return 1;
    }
    if (client.statusCode == 201)
    {
        client.state_string = "201 created";
        client.fullPath = get_page(client, 201);
        return 1;
    }
    return 0;
}
