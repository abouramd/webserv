#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <vector>

std::vector<std::string> my_split(const std::string str, const std::string lim);

long strtoint(std::string &str, long m) {
  long buff = 0, pre = 0;
  std::string::iterator it = str.begin();

  if (it == str.end())
    return -1;

  while (it != str.end()) {
    if (!std::isdigit(*it))
      return -1;
    buff = buff * 10 + *it - '0';
    if (buff > m || buff < pre)
      return -1;
    pre = buff;
    it++;
  }
  return buff;
}

int iptorin(const std::string str, in_addr *addr) {
  if (std::count(str.begin(), str.end(), '.') != 3)
    return 0;
  std::vector<std::string> vec = my_split(str, ".");
  if (vec.size() != 4)
    return 0;

  int tmp;
  unsigned int ip = 0;
  int bit = 24;

  for (int i = 0; i < 4; i++) {
    tmp = strtoint(vec[i], 255);
    if (tmp == -1)
      return 0;
    ip |= tmp << bit;
    bit -= 8;
  }
  if (addr)
    addr->s_addr = htonl(ip);
  return 1;
}

std::string bintoip(in_addr addr) {
  unsigned int ip = ntohl(addr.s_addr);

  std::stringstream ss;

  int bit = 24;

  for (int i = 0; i < 4; i++) {

    ss << (255 & (ip >> bit));
    if (bit)
      ss << '.';
    bit -= 8;
  }
  return ss.str();
}
