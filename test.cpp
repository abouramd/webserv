#include <iostream>
#include <map>

using namespace std;

int main(int argc, char *argv[]) {
  std::map<string, string> mymap;
  mymap["/upload/"] = "1";
  mymap["/upload"] = "2";
  mymap["/upload//"] = "3";

  std::map<string, string>::iterator i = mymap.end();
  while (i-- != mymap.begin()) {
    cout << i->second << "\n";
  }
  return 0;
}
