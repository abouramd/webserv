#include <iostream>
#include <sstream>
#define BOUND 1
#define HEAD 2
#define BOD 3

void    unBound(std::string request) {
    std::stringstream   ss;
    std::string         buf(request);
    size_t              pos = 0;

    ss << request.boundBuf;
    request.boundBuf.clear();
    ss << buf.substr(request.position);
    buf.clear();
    while (getline(ss, buf)) {
        if (buf == "--" + request.boundary + "--")
            throw 201;
        if (ss.eof()) {
            request.boundBuf = buf;
            return;
        }
        if (request.boundState != HEAD && buf == "--" + request.boundary)
            request.boundState = HEAD;
        else if (request.boundState == HEAD) {
            if (buf == "\r")
                request.boundState = BOD;
        }
        else if (request.boundState == BOD)
            request.outfile << buf;
    }
}

int main() {
    std::string req("-----------------------------974767299852498929531610575\r\n"
                    "Content-Disposition: form-data; name=\"description\"\r\n"
                    "\r\n"
                    "some text\r\n"
                    "-----------------------------974767299852498929531610575\r\n"
                    "Content-Disposition: form-data; name=\"myFile\"; filename=\"foo.txt\"\r\n"
                    "Content-Type: text/plain\r\n"
                    "\r\n"
                    "(content of the uploaded file foo.txt)\r\n"
                    "-----------------------------974767299852498929531610575--");
    bound(req, "-----------------------------974767299852498929531610575");
}