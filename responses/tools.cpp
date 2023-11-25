#include "responses.hpp"
#include <cstring>

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


int s_header(Client& client, int client_socket, std::string status, std::string type)
{
  int n;
	std::string head = "HTTP/1.1 " + status + "\r\n";
	head += cur_time() + "\r\n";
	head += "Server: webserver (abouramd)\r\n";
	head += "Content-Type: " + type + "\r\n";
	head += "Cache-Control: no-store, no-cache, must-revalidate\r\n";
	head += "Transfer-Encoding: chunked\r\n";	
	head += "Connection: close\r\n";	
  head += "\r\n";
  n = write(client_socket, (char *)head.c_str(), head.size());
	if (n == -1)
  {
    client.state = CLOSE;
  }
  if (!n)
    return 1;
  return 0;
}

void c_base( std::string& str, int n, const int &base)
{
  if (n >= base)
    c_base(str, n / base, base);
  str += "0123456789abcdef"[n % base];
}


void s_chank(Client& client, int fd, const char *content, const int size)
{
  int n;
  std::string count;
	c_base(count, size, 16);
	count += "\r\n";
  unsigned int new_size = count.size() + size + 2;
  char s[new_size];
  std::memcpy(s, count.c_str(), count.size());
  std::memcpy(s + count.size(), content, size);
  std::memcpy(s + new_size - 2, "\r\n", 2);
  n = write(fd, s, new_size);
	if (n == -1)
    client.state = CLOSE;
  else if(!n)
    client.write_f = 5;
  else
    client.write_f = 0;
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
    if (client.location.second.cgi.first)
    {
        if (client.location.second.cgi.second.find(get_ex(client.fullPath)) != client.location.second.cgi.second.end())
            return 1;
    }
    return 0;
}

int get_index(Client &client)
{
    int i = 0;
    std::string index;
    while(i < (int)client.location.second.index.size())
    {
        index = client.fullPath + "/" + client.location.second.index[i];
        if (!access(index.c_str(), F_OK | R_OK) && !is_dir(index))
        {
            client.fullPath = index;
            return 1;
        }
        i++;
    }
    return 0;
}



void redirect(Client &client, std::string target)
{
    std::string redi;
    redi = "HTTP/1.1 301 Moved Permanently\r\nLocation: " + target + "\r\n\r\n";
    int n = write(client.fd, redi.c_str(), redi.size());
    if (n == -1)
      client.state = CLOSE;
    if (n == 0)
      client.state = DONE;
}

int check_header(std::string header)
{
    std::vector<std::string> headers;
    std::string key = "";
    int i = 0;

    while(i < (int)header.size())
    {
        key += header[i];
        if (header[i] == ':')
            break;
        if (i == 29)
            return 0;
        i++;
    } 
    headers.push_back(Tools::toLower("Accept-CH:"));
    headers.push_back(Tools::toLower("Access-Control-Allow-Origin:"));
    headers.push_back(Tools::toLower("Accept-Patch:"));
    headers.push_back(Tools::toLower("Accept-Ranges:"));
    headers.push_back(Tools::toLower("Age:"));
    headers.push_back(Tools::toLower("Allow:"));
    headers.push_back(Tools::toLower("Alt-Svc:"));
    headers.push_back(Tools::toLower("Cache-Control:"));
    headers.push_back(Tools::toLower("Connection:"));
    headers.push_back(Tools::toLower("Content-Disposition:"));
    headers.push_back(Tools::toLower("Content-Encoding:"));
    headers.push_back(Tools::toLower("Content-Language:"));
    headers.push_back(Tools::toLower("Content-Length:"));
    headers.push_back(Tools::toLower("Content-Location:"));
    headers.push_back(Tools::toLower("keyContent-MD5:"));
    headers.push_back(Tools::toLower("Content-Range:"));
    headers.push_back(Tools::toLower("Content-Type:"));
    headers.push_back(Tools::toLower("Date:"));
    headers.push_back(Tools::toLower("Delta-Base:"));
    headers.push_back(Tools::toLower("ETag:"));
    headers.push_back(Tools::toLower("Expires:"));
    headers.push_back(Tools::toLower("IM:"));
    headers.push_back(Tools::toLower("Last-Modified:"));
    headers.push_back(Tools::toLower("Link:"));
    headers.push_back(Tools::toLower("Location:"));
    headers.push_back(Tools::toLower("P3P:"));
    headers.push_back(Tools::toLower("Pragma:"));
    headers.push_back(Tools::toLower("Preference-Applied:"));
    headers.push_back(Tools::toLower("Proxy-Authenticate:"));
    headers.push_back(Tools::toLower("Public-Key-Pins:"));
    headers.push_back(Tools::toLower("Retry-After:"));
    headers.push_back(Tools::toLower("Server:"));
    headers.push_back(Tools::toLower("Set-Cookie:"));
    headers.push_back(Tools::toLower("Strict-Transport-Security:"));
    headers.push_back(Tools::toLower("Trailer:"));
    headers.push_back(Tools::toLower("Transfer-Encoding:"));
    headers.push_back(Tools::toLower("Tk:"));
    headers.push_back(Tools::toLower("Upgrade:"));
    headers.push_back(Tools::toLower("Vary:"));
    headers.push_back(Tools::toLower("Via:"));
    headers.push_back(Tools::toLower("Warning:"));
    headers.push_back(Tools::toLower("WWW-Authenticate:"));
    headers.push_back(Tools::toLower("X-Frame-Options:"));
    headers.push_back(Tools::toLower("X-WebKit-CSP:"));
    headers.push_back(Tools::toLower("Expect-CT:"));
    headers.push_back(Tools::toLower("NEL:"));
    headers.push_back(Tools::toLower("Permissions-Policy:"));
    headers.push_back(Tools::toLower("Refresh:"));
    headers.push_back(Tools::toLower("Report-To:"));
    headers.push_back(Tools::toLower("Status:"));
    headers.push_back(Tools::toLower("Timing-Allow-Origin:"));
    headers.push_back(Tools::toLower("X-Content-Duration:"));
    headers.push_back(Tools::toLower("X-Content-Type-Options:"));
    headers.push_back(Tools::toLower("X-Powered-By:"));
    headers.push_back(Tools::toLower("X-Redirect-By:"));
    headers.push_back(Tools::toLower("X-Request-ID:"));
    headers.push_back(Tools::toLower("X-UA-Compatible:"));
    headers.push_back(Tools::toLower("X-XSS-Protection:"));
    if (std::find(headers.begin(), headers.end(), Tools::toLower(key)) != headers.end())
        return 1;
    return 0;
}

