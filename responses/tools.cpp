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
	head += "Cache-Control: no-store, no-cache, must-revalidate\r\n";
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
            std::cout << "Here: "<< index << std::endl;
            client.fullPath = index;
            return 1;
        }
        i++;
    }
    return 0;
}

int auto_index(Client &client)
{
    if (client.opened != 5)
    {
        s_header(client.fd, "200 OK", "text/html");
        std::string head = "<!DOCTYPE html><html><head><title>Index of "+client.path+"</title><style>body,ul{padding:20px}a,li strong{font-weight:700}body,ul{margin:0}body{background-color:#f8f8f8;font-family:Arial,sans-serif;display:flex;flex-direction:column;align-items:center;justify-content:center;min-height:100vh}h1{background-color:#333;border:2px solid #3498db;border-radius:10px;color:#fff;padding:10px;text-align:center;font-size:24px}ul{background-color:#fff;border:2px solid #3498db;border-radius:10px;box-shadow:0 0 15px rgba(0,0,0,.2);width:80%;list-style-type:none}li{margin-bottom:10px;padding:8px;border:1px solid #ccc;border-radius:5px;background-color:#f2f2f2}li strong{color:#e74c3c}a{text-decoration:none;color:#3498db;opacity:1;transition:opacity 1s}a:hover{text-decoration:underline;color:#4ce73c;opacity:.6}</style></head><body><h1>Index of "+client.path+"</h1><ul>";
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
                    std::string dir = "<li><a href='./"  + std::string(entry->d_name) + "'>" + std::string(entry->d_name) + "</a></li>";
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

int check_header(std::string header)
{
    std::vector<std::string> headers;
    std::string key = "";
    int i = 0;

    while(i < (int)header.size())
    {
        if (header[i] == ':')
            break;
        key += header[i];
        i++;
    }

    headers.push_back(Tools::toLower("Accept-CH"));
    headers.push_back(Tools::toLower("Access-Control-Allow-Origin"));
    headers.push_back(Tools::toLower("Accept-Patch"));
    headers.push_back(Tools::toLower("Accept-Ranges"));
    headers.push_back(Tools::toLower("Age"));
    headers.push_back(Tools::toLower("Allow"));
    headers.push_back(Tools::toLower("Alt-Svc"));
    headers.push_back(Tools::toLower("Cache-Control"));
    headers.push_back(Tools::toLower("Connection"));
    headers.push_back(Tools::toLower("Content-Disposition"));
    headers.push_back(Tools::toLower("Content-Encoding"));
    headers.push_back(Tools::toLower("Content-Language"));
    headers.push_back(Tools::toLower("Content-Length"));
    headers.push_back(Tools::toLower("Content-Location"));
    headers.push_back(Tools::toLower("keyContent-MD5"));
    headers.push_back(Tools::toLower("Content-Range"));
    headers.push_back(Tools::toLower("Content-Type"));
    headers.push_back(Tools::toLower("Date"));
    headers.push_back(Tools::toLower("Delta-Base"));
    headers.push_back(Tools::toLower("ETag"));
    headers.push_back(Tools::toLower("Expires"));
    headers.push_back(Tools::toLower("IM"));
    headers.push_back(Tools::toLower("Last-Modified"));
    headers.push_back(Tools::toLower("Link"));
    headers.push_back(Tools::toLower("Location"));
    headers.push_back(Tools::toLower("P3P"));
    headers.push_back(Tools::toLower("Pragma"));
    headers.push_back(Tools::toLower("Preference-Applied"));
    headers.push_back(Tools::toLower("Proxy-Authenticate"));
    headers.push_back(Tools::toLower("Public-Key-Pins"));
    headers.push_back(Tools::toLower("Retry-After"));
    headers.push_back(Tools::toLower("Server"));
    headers.push_back(Tools::toLower("Set-Cookie"));
    headers.push_back(Tools::toLower("Strict-Transport-Security"));
    headers.push_back(Tools::toLower("Trailer"));
    headers.push_back(Tools::toLower("Transfer-Encoding"));
    headers.push_back(Tools::toLower("Tk"));
    headers.push_back(Tools::toLower("Upgrade"));
    headers.push_back(Tools::toLower("Vary"));
    headers.push_back(Tools::toLower("Via"));
    headers.push_back(Tools::toLower("Warning"));
    headers.push_back(Tools::toLower("WWW-Authenticate"));
    headers.push_back(Tools::toLower("X-Frame-Options"));
    headers.push_back(Tools::toLower("X-WebKit-CSP"));
    headers.push_back(Tools::toLower("Expect-CT"));
    headers.push_back(Tools::toLower("NEL"));
    headers.push_back(Tools::toLower("Permissions-Policy"));
    headers.push_back(Tools::toLower("Refresh"));
    headers.push_back(Tools::toLower("Report-To"));
    headers.push_back(Tools::toLower("Status"));
    headers.push_back(Tools::toLower("Timing-Allow-Origin"));
    headers.push_back(Tools::toLower("X-Content-Duration"));
    headers.push_back(Tools::toLower("X-Content-Type-Options"));
    headers.push_back(Tools::toLower("X-Powered-By"));
    headers.push_back(Tools::toLower("X-Redirect-By"));
    headers.push_back(Tools::toLower("X-Request-ID"));
    headers.push_back(Tools::toLower("X-UA-Compatible"));
    headers.push_back(Tools::toLower("X-XSS-Protection"));
    if (std::find(headers.begin(), headers.end(), Tools::toLower(key)) != headers.end())
    {
        return 1;
    }
    return 0;
}