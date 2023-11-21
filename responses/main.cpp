#include "responses.hpp"
#include <csignal>

void responses(Client &client) {
  int n;
  if (!client.is->is_open()) {
    client.state_string = "200 OK";
    error_handling(client);
    if (client.method == "GET") {
      if (client.location.second.redirect.empty())
        get(client);
      else {
        redirect(client, client.location.second.redirect);
      }
    } else if (client.method == "DELETE") {
      ft_delete(client);
    }
  } else {
    if (client.is_cgi != 5 || (waitpid(client.pid, &client.pro_state, WNOHANG))) {
      if (client.is_cgi == 5) {
        if (WEXITSTATUS(client.pro_state) == 100) {
          client.statusCode = 500;
          client.is_cgi = 4;
          client.is->close();
          return;
        }
        std::string header; // = "HTTP/1.1 200 OK\r\n";
        std::string head;
        bool ct(true);
        while (std::getline(*client.is, head) && head != "\r" && head != "") {
          if (!Tools::toLower(head).compare(0, std::strlen("status: "),
                                            "status: "))
            head = "HTTP/1.1 " + head.substr(7);
          else if (header.empty()) {
            header += "HTTP/1.1 200 OK\r\n";
            if (head.empty() || !check_header(head)) {
              client.is->close();
              client.is->open(client.cgiFileName.c_str());
              break;
            }
          }

          if (Tools::toLower(head).compare(0, std::strlen("content-type: "),
                                           "content-type: "))
            ct = false;
          header += head;
          header += "\n";
        }
        if (header.empty())
          header += "HTTP/1.1 200 OK\r\n";
        if (ct)
          header += "Content-type: text/html\r\n";
        header += "Transfer-Encoding: chunked\r\n";
        header += "\r\n";
        n = write(client.fd, header.c_str(), header.size());
        if (n == -1) {
          client.is->close();
          remove(client.cgiFileName.c_str());
          client.state = CLOSE;
        }
        if (!n)
          return;
        client.is_cgi = 4;
        return;
      }
      if (client.write_f != 5) {
        client.is->read(client.buf, 1024);
        client.buffSize = client.is->gcount();
      } else
        client.write_f = 0;
      if (client.buffSize) {
        s_chank(client, client.fd, client.buf, client.buffSize);
      } else {
        s_chank(client, client.fd, "", 0);
        client.is->close();
        client.state = CLOSE;
        if (client.is_cgi == 4 || client.is_cgi == 5)
          remove(client.cgiFileName.c_str());
      }
    }
    else
      client.check_cgi++;
    if (client.is_cgi == 5) {
      std::time_t currentTime = time(NULL);
      if (currentTime - client.currentTime > 5) {
        client.statusCode = 408;
        client.is_cgi = 4;
        client.is->close();
        kill(client.pid, SIGKILL);
        waitpid(client.pid, NULL, 0);
        remove(client.cgiFileName.c_str());
        return;
      }
    }
  }
}
