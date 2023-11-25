#include "responses.hpp"
#include <string>

int auto_index(Client &client)
{
  if (!client.chank.empty())
  {
    s_chank(client, client.fd, client.chank.c_str(), client.chank.size());
    if (!client.write_f)
      client.chank.clear();
  }
  else if (client.opened == 0)
  {
        client.dir = opendir(client.fullPath.c_str());
        if (!client.dir)
          client.statusCode = 403;
        client.opened = 1;
  }
  else if (client.opened == 1)
  {
        if (s_header(client, client.fd, "200 OK", "text/html"))
            return 0;
        client.opened = 2;
  }
  else if (client.opened == 2)
  { 
        std::string head = "<!DOCTYPE html><html><head><title>Index of "+client.path+"</title><style>body,ul{padding:20px}a,li strong{font-weight:700}body,ul{margin:0}body{background-color:#f8f8f8;font-family:Arial,sans-serif;display:flex;flex-direction:column;align-items:center;justify-content:center;min-height:100vh}h1{background-color:#333;border:2px solid #3498db;border-radius:10px;color:#fff;padding:10px;text-align:center;font-size:24px}ul{background-color:#fff;border:2px solid #3498db;border-radius:10px;box-shadow:0 0 15px rgba(0,0,0,.2);width:80%;list-style-type:none}li{margin-bottom:10px;padding:8px;border:1px solid #ccc;border-radius:5px;background-color:#f2f2f2}li strong{color:#e74c3c}a{text-decoration:none;color:#3498db;opacity:1;transition:opacity 1s}a:hover{text-decoration:underline;color:#4ce73c;opacity:.6}</style><link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.1/font/bootstrap-icons.css\"></head><body><h1>Index of "+client.path+"</h1><ul>";
        s_chank(client, client.fd, head.c_str(), head.size());    
        if (client.write_f != 5)
          client.opened = 3;
  }
  else if (client.opened == 3)
  {
    int i = 0;
    struct dirent* entry;
    while (i < 6 && (entry = readdir(client.dir)))
    {
      if (entry->d_name[0] != '.')
      {
        struct stat filestat;
        stat(std::string(client.fullPath + "/" + entry->d_name).c_str(),&filestat);
        if( S_ISDIR(filestat.st_mode) )
          client.chank += "<li><a href='./"  + std::string(entry->d_name) + "'><i class='bi bi-folder-fill'></i> " + std::string(entry->d_name) + "</a></i></li>";
        else
          client.chank += "<li><a href='./"  + std::string(entry->d_name) + "'><i class='bi bi-file-earmark-text-fill'></i> " + std::string(entry->d_name) + "</a></i></li>";
        i++;
      }
    }
    if (entry == NULL)
    {
      client.chank += "</ul></body></html>";
      client.opened = 4;
    }
    s_chank(client, client.fd, client.chank.c_str(), client.chank.size());
    if (!client.write_f)
      client.chank.clear();
  }
  else
  {
    s_chank(client, client.fd, "", 0);
    if (client.write_f != 5)
    {
      client.state = CLOSE;
      closedir(client.dir);
      client.dir = NULL;
    }
  }
  return 0;
}