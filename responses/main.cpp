#include "responses.hpp"

void responses(Client &client)
{
    char buffer[100];
    std::string ftarget = client.target;
    std::string get_query = "";
    if (!client.is->is_open())
    {
        get_target(client, get_query);
        if (client.method == "GET")
        {
            if (client.server->second.redirect.empty())
                get(client, get_query, ftarget);
            else
                redirect(client, client.server->second.redirect);
        }
    }
    else
    {
        if (client.is_cgi != 5 || waitpid(client.pid, NULL, WNOHANG))
        {
            if (client.is_cgi == 5)
            {
                std::string header = "HTTP/1.1 200 OK\r\n";
                header += "Transfer-Encoding: chunked\r\n";
                std::string head;
                while (std::getline(*client.is, head) && head != "\r" && head != "")
                {
                    header += head;
                    header += "\r\n";
                }
                header +="\r\n";
                write(client.fd, header.c_str(), header.size());
                client.is_cgi = 4;
            }
            client.is->read(buffer, sizeof(buffer));
            if (client.is->gcount())
            {
                s_chank(client.fd, buffer, client.is->gcount());
            }
            else
            {
                s_chank(client.fd, "", 0);
                client.is->close();
                client.state = CLOSE;
                if (client.is_cgi == 4 || client.is_cgi == 5)
                    remove(client.method.c_str());
            }
        }
    }
}