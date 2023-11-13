#include "Tools.hpp"

void    boundS(Client & request) {
    while (request.position < request.buffSize && request.buf[request.position] != '\n') {
        request.boundBuf += request.buf[request.position];
        request.position++;
        if (request.boundBuf.size() >= request.boundary.size() + 5)
            break;
    }
    if (request.position < request.buffSize || request.boundBuf.size() >= request.boundary.size() + 5) {
        if (request.boundBuf == ("--" + request.boundary + "--\r").c_str())
            throw 201;
        else if (request.boundBuf == ("--" + request.boundary + "\r").c_str()) {
            request.boundBuf.clear();
            request.boundState = HEAD;
            request.outfile->close();
            request.position++;
        }
        else {
            *(request.outfile) << request.boundBuf;
            request.boundState = BOD;
            request.boundBuf.clear();
        }
    }
}

void    bodS(Client & request) {
    if (request.outfile->is_open() && request.buf[request.position] == '-')
        request.boundState = BOUND;
    else if (request.outfile->is_open()) {
        *(request.outfile) << request.buf[request.position];
        request.position++;
    }
    else if (!request.outfile->is_open()) {
        std::string extension, uploadPath;

        uploadPath = request.location.second.root + request.location.second.uplode.second;
        if (request.contentType.size())
            extension = FileType::getExt(std::string(request.contentType.c_str()));
        Tools::getAndCheckPath(uploadPath, extension);
        if (request.contentType.size())
            request.outfile->open(uploadPath.c_str());
        else {
            std::stringstream   ss;

            ss << request.location.second.root + request.location.second.uplode.second + "/" << rand() << ".file";
            extension = ss.str();
            request.outfile->open(extension.c_str());
            ss.str("");
            ss.clear();
        }
        request.contentType.clear();
    }
}

void    headS(Client & request) {
    while (request.position < request.buffSize && request.buf[request.position] != '\n') {
        request.boundBuf += request.buf[request.position];
        request.position++;
    }
    if (request.position < request.buffSize) {
        if (request.boundBuf == "\r")
            request.boundState = BOD;
        else {
            size_t  pos = request.boundBuf.match("content-type");

            if (pos != std::string::npos && request.boundBuf.size() > 15)
                request.contentType = String(request.boundBuf.c_str(), 14, request.boundBuf.size() - 15);
        }
        request.position++;
        request.boundBuf.clear();
    }
}

void    unBound(Client & request) {
    while (request.position < request.buffSize) {
        if (request.boundState == BOUND)
            boundS(request);
        else if (request.boundState == BOD)
            bodS(request);
        else if (request.boundState == HEAD)
            headS(request);
    }
}