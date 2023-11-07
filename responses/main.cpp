#include "responses.hpp"

void responses(Client &client)
{
    char buffer[100];
    if (!client.is->is_open())
    {
        std::string ftarget = client.location->first + client.target;
        client.state_string = "200 OK";
        error_handling(client);
        if (client.method == "GET")
        {
            if (client.location->second.redirect.empty())
                get(client, ftarget);
            else
                redirect(client, client.location->second.redirect);
        }
        else if (client.method == "DELETE")
        {
            ft_delete(client);
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
                if (!(get_ex(client.fullPath).compare("php")))
                {
                    while (std::getline(*client.is, head) && head != "\r" && head != "")
                    {
                        header += head;
                        header += "\n";
                    }
                }
                else
                    header += "Content-type: text/html\r\n";
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
        if (client.is_cgi == 5)
        {
            std::time_t currentTime = time(NULL);
            if (currentTime - client.currentTime > 10)
            {
                s_header(client.fd, "408 Timed out", "text/html");
                client.is->close();
                client.is->open("error_pages/408.html");
                kill(client.pid, SIGINT);
                remove(client.method.c_str());
                client.is_cgi = 4;
            }
        }
    }
}
