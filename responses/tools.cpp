#include "responses.hpp"

std::string cur_time() {
    // Get the current time in GMT
    std::time_t currentTime = std::time(NULL);

    // Convert the time to a struct tm (timeinfo) in GMT
    struct std::tm* timeinfo = std::gmtime(&currentTime);

    // Format the time as a string
    char buffer[200];
    strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %H:%M:%S GMT", timeinfo);

    return std::string(buffer);
}


void s_header(int client_socket, std::string status, std::string type)
{
	std::string head = "HTTP/1.1 " + status + "\r\n";
	head += cur_time() + "\r\n";
	head += "Server: webserver (abouramd)\r\n";
	head += "Content-Type: " + type + "\r\n";
	head += "Transfer-Encoding: chunked\r\n";	
    head += "\r\n";
	// std::cout << head << std::endl;
	write(client_socket, (char *)head.c_str(), head.size());
    // write(1, (char *)head.c_str(), head.size());
}

void c_base(std::string& str, int n, const int &base)
{
  if (n >= base)
    c_base(str, n / base, base);
  str += "0123456789abcdef"[n % base];
}


void s_chank(int fd, const char *content, const int size)
{
	std::string count;
	c_base(count, size, 16);
	count += "\r\n";
	write(fd, count.c_str(), count.size());
	write(fd, content, size);
	write(fd, "\r\n", 2);
    // write(1, count.c_str(), count.size());
	// write(1, content, size);
	// write(1, "\r\n", 2);
}

int is_dir(std::string& str)
{
    struct stat st;

    stat(str.c_str(), &st);
    if (access(str.c_str(), F_OK) == 0) {
        if (S_ISREG(st.st_mode)) {
            return 0;
        } else if (S_ISDIR(st.st_mode)) {
            return 1;
        }
    }
    return 2;
}



std::string get_ex(std::string str)
{
    int i = 0, count = 0, count2 = 0;
    std::string s = "";

    while(str[i])
    {
        if (str[i] == '.')
            count++;
        i++;
    }
    i = 0;
    while(str[i])
    {
        if (str[i] == '.')
            count2++;
        else if (count2 == count)
            s += str[i];
        i++;
    }
    return s;
}

int is_cgi(Client &client)
{
    if (client.location->second.cgi.first)
    {
        if (client.location->second.cgi.second.find(get_ex(client.fullPath)) != client.location->second.cgi.second.end())
            return 1;
    }
    return 0;
}

int get_index(Client &client)
{
    int i = 0;
    std::string index;
    while(i < (int)client.location->second.index.size())
    {
        index = client.fullPath + "/" + client.location->second.index[i];
        if (access(index.c_str(), F_OK | R_OK) == 0)
        {
            client.fullPath = index;
            return 1;
        }
        i++;
    }
    return 0;
}

int auto_index(Client &client, std::string &ftarget)
{
    if (client.opened != 5)
    {
        s_header(client.fd, "200 OK", "text/html");
        std::string head = "<!DOCTYPE html><html><head><title>Index of "+ftarget+"</title><style>body {font-family: Arial, sans-serif;}h1 {text-align: center; font-size: 150px}ul {    list-style-type: none; text-align: center;    padding: 0; font-size: 100px}li {    margin: 5px 0;}li a {    text-decoration: none;    color: #0074d9;}li a:hover {    text-decoration: underline;}</style></head><body><h1>Index of "+ftarget+"</h1><ul>";
        s_chank(client.fd, head.c_str(), head.size());
        client.dir = opendir(client.fullPath.c_str());
        client.opened = 5;
    }
    else
    {
        if (client.dir) 
        {
            struct dirent* entry;
            if ((entry = readdir(client.dir)))
            {
                if (entry->d_name[0] != '.')
                {
                    std::string dir = "<li><a href='" + std::string(entry->d_name) + "'>" + std::string(entry->d_name) + "</a></li>";
                    s_chank(client.fd, dir.c_str(), dir.size());
                }
            }
            else
            {
                std::string foot = "</ul></body></html>";
                s_chank(client.fd, foot.c_str(), foot.size());
                s_chank(client.fd, "", 0);
                client.state = CLOSE;
                closedir(client.dir);
            }
        }
        else{
            s_header(client.fd, "403 Forbidden", "text/html");
            client.is->open("error_pages/403.html");
        }
    }
    return 0;
}

void redirect(Client &client, std::string target)
{
    write(client.fd, "HTTP/1.1 301 Moved Permanently\r\nLocation: ", 42);
    write(client.fd, target.c_str(), target.length());
    write(client.fd, "\r\n\r\n", 4);
    client.state = CLOSE;
}