#include "Tools.hpp"

void    getSize(Client & request) {
    std::string hex("ABCDEFabcdef0123456789");


    if (hex.find(request.buf[request.position]) == std::string::npos) {
        if (request.chunkSizeStr.empty())
            throw 400;
        request.chunkSizeNum = std::strtol(request.chunkSizeStr.c_str(), NULL, 16);
        if (request.chunkSizeNum == 0)
            throw 201;
        request.contentLength += request.chunkSizeNum;
        if (request.contentLength > request.maxBodySize)
            throw 413;
        request.chunkSizeStr.clear();
        request.chState = CRLF_CH;
        request.chNext = CHUNK;
    }
    else {
        request.chunkSizeStr += std::tolower(request.buf[request.position]);
        request.position++;
    }
}

void    checkCrlfCh(Client & request) {

    request.crlf += request.buf[request.position];

    if (request.crlf.size() == 2) {
        if (request.crlf != "\r\n")
            throw 400;
        request.chState = request.chNext;
        request.crlf.clear();
    }
    request.position++;
}

void    unChunk(Client & request) {
    if (request.chunkSizeNum > request.buffSize - request.position) {
        request.outfile->write(request.buf + request.position, request.buffSize - request.position);
        request.outfile->flush();
        request.chunkSizeNum -= request.buffSize - request.position;
        request.position = request.buffSize;
    }
    else {
        request.outfile->write(request.buf + request.position, request.chunkSizeNum);
        request.outfile->flush();
        request.position += request.chunkSizeNum;
        request.chunkSizeNum = 0;
        request.chState = CRLF_CH;
        request.chNext = SIZE;
    }
}

void    handleChunked(Client & request) {
    switch (request.chState) {
        case SIZE:
            getSize(request);
            break;
        case CRLF_CH:
            checkCrlfCh(request);
            break;
        case CHUNK:
            unChunk(request);
            break;
    }
}