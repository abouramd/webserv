#include <iostream>
#include <map>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
  std::map<string, string> mymap;
  mymap["/u"] = "1";
  mymap["/upload"] = "2";
  mymap["/upload//"] = "3";

  std::string url = "/upload//hello";
  std::map<string, string>::iterator i = mymap.end();
  while (i-- != mymap.begin()) {
    if (!url.compare(0, i->first.length(), i->first))
      cout << i->second << "\n";
  }
  return 0;
}
