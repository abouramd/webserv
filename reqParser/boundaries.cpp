#include "Tools.hpp"

void    checkFirstBoundary(Client & request) {
    while (request.position < request.buffSize && request.boundBuf.size() < request.boundary.size() + 2) {
        if (request.buf[request.position] != (request.boundary + "\r\n").c_str()[request.boundBuf.size()])
            throw 400;
        request.boundBuf += request.buf[request.position];
        request.position++;
    }
    if (request.boundBuf.size() == request.boundary.size() + 2)
        request.boundState = HEAD;
}

void    boundS(Client & request) {
    std::string newBoundary("\r\n" + request.boundary), lastTwo;
    size_t      startSize = request.boundBuf.size(), size = newBoundary.size() + 2;
    bool        breaked = false;

    while (request.position < request.buffSize && startSize < size) {
        if (startSize < size - 2) {
            if (request.buf[request.position] != newBoundary[request.boundBuf.size()]) {
                breaked = true;
                break;
            }
            request.boundBuf += request.buf[request.position];
        }
        else
            lastTwo += request.buf[request.position];
        request.position++;
        startSize++;
    }
    if (lastTwo == "--")
        throw 201;
    else if (lastTwo == "\r\n") {
        request.boundBuf.clear();
        request.boundState = HEAD;
        request.outfile->close();
        return;
    }
    else if (breaked || lastTwo.size() == 2) {
        request.outfile->write(request.boundBuf.c_str(), request.boundBuf.size());
        request.boundState = BOD;
        request.boundBuf.clear();
    }
}

void    bodS(Client & request) {
    size_t  posOfBound, size;

    posOfBound = Tools::findBin(request.buf, '\r', request.position, request.buffSize - request.position);
    if (posOfBound == std::string::npos)
        size = request.buffSize - request.position;
    else {
        request.boundState = BOUND;
        size = posOfBound - request.position;
    }
    if (!request.outfile->is_open()) {
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
    request.outfile->write(&request.buf[request.position], size);
    request.position = posOfBound != std::string::npos ? posOfBound : request.buffSize;
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
        if (request.boundState == AT_START)
            checkFirstBoundary(request);
        if (request.boundState == BOUND)
            boundS(request);
        else if (request.boundState == BOD)
            bodS(request);
        else if (request.boundState == HEAD)
            headS(request);
    }
}