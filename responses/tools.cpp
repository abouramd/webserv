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

void get_target(Client &client)
{
    size_t found = client.target.find(client.server->first);
    if (found != std::string::npos)
        client.target.replace(found, client.server->first.length(), client.server->second.root);
}