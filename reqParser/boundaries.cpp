#include "Tools.hpp"

void    checkFirstBoundary(Client & request) {
    request.contentLength = 0;
    while (request.position < request.buffSize && request.boundBuf.size() < request.boundary.size() + 2) {
        if (request.buf[request.position] != (request.boundary + "\r\n").c_str()[request.boundBuf.size()])
            throw 400;
        request.boundBuf.push_back(request.buf[request.position]);
        request.position++;
    }
    if (request.boundBuf.size() == request.boundary.size() + 2)
        request.boundState = HEAD;
}

void    boundS(Client & request) {
    size_t      size = request.boundary.size() + 4, save;

    if (request.boundBuf.size() + request.buffSize - request.position >= size) {
      save = size - request.boundBuf.size();
      request.boundBuf.insert(request.boundBuf.end(), request.buf + request.position, request.buf + request.position + save);
      request.position += save;
    }
    else {
      request.boundBuf.insert(request.boundBuf.end(), request.buf + request.position, request.buf + request.buffSize);
      request.position = request.buffSize;
    }
    if (request.boundBuf.size() == size) {
      if (std::string(request.boundBuf.begin(), request.boundBuf.end()) == "\r\n" + request.boundary + "--") {
        request.boundBuf.clear();
        throw 201;
      }
      else if (std::string(request.boundBuf.begin(), request.boundBuf.end()) == "\r\n" +request.boundary + "\r\n") {
        request.boundBuf.clear();
        request.boundState = HEAD;
        request.outfile->close();
        return;
      }
      else {
        request.contentLength += request.boundBuf.size();
        if (request.contentLength > request.maxBodySize) {
          std::remove(request.uploadFile.c_str());
          throw 413;
        }
        request.outfile->write(&request.boundBuf[0], request.boundBuf.size());
        request.boundState = BOD;
        request.boundBuf.clear();
      }
    }
}

void    bodS(Client & request) {
    size_t  posOfBound, size;


    posOfBound = Tools::findBin(request.buf, '\r', request.position, request.buffSize);
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
        request.uploadFile = uploadPath;
        if (request.contentType.size())
            request.outfile->open(uploadPath.c_str());
        else {
            std::stringstream   ss;

            ss << request.location.second.root + request.location.second.uplode.second + "/" << rand() << ".file";
            request.uploadFile = ss.str();

            request.outfile->open(request.uploadFile.c_str());
            ss.str("");
            ss.clear();
        }
        request.contentType.clear();
    }
    request.contentLength += size;
    if (request.contentLength > request.maxBodySize) {
        std::remove(request.uploadFile.c_str());
        throw 413;
    }
    request.outfile->write(&request.buf[request.position], size);
    request.position = posOfBound != std::string::npos ? posOfBound : request.buffSize;
}

void    headS(Client & request) {
    while (request.position < request.buffSize && request.buf[request.position] != '\n') {
        request.boundBuf.push_back(request.buf[request.position]);
        request.position++;
    }
    if (request.position < request.buffSize) {
        std::string header(request.boundBuf.begin(), request.boundBuf.end());

        if (header == "\r")
            request.boundState = BOD;
        else if (Tools::toLower(header).find("content-type:") != std::string::npos) {
          std::stringstream ss(header);
          ss >> request.contentType >> request.contentType;
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
