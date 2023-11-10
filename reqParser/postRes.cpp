#include "Tools.hpp"

bool getSize(Client & request, size_t & st) {
    std::stringstream   sizeStream;
    int start = st;

    if (!request.sizeDept.empty()) {
        sizeStream << request.sizeDept;
        request.sizeDept.clear();
    }
    while (st < request.buffSize && request.buf[st] != '\r')
        sizeStream << request.buf[st++];
    if (st == request.buffSize) {
        request.sizeDept.assign(request.buf + start, st - start);
        request.position = 0;
        return false;
    }
    st += 2;
    sizeStream >> std::hex >> request.chunkSize;
    return true;
}

void    unCh(Client & request) {
    bool    check;
    size_t  st;

    if (request.chunkSize) {
        request.outfile->write(request.buf, request.chunkSize);
        request.position = request.chunkSize + 2;
        request.chunkSize = 0;
    }
    if (request.position >= request.buffSize) {
        request.position -= request.buffSize;
        return;
    }
    st = request.position;

    while (st < request.buffSize) {
        check = getSize(request, st);

        if (!check)
            break;
        if (request.chunkSize == 0)
            throw 201;
        if (st >= request.buffSize) {
            request.position = st - request.buffSize;
            break;
        }
        while (request.chunkSize > 0 && st < request.buffSize) {
            *(request.outfile) << request.buf[st];
            request.chunkSize--;
            st++;
        }
        if (request.chunkSize)
            break;
        st += 2;
        if (st >= request.buffSize) {
            request.position = st - request.buffSize;
            break;
        }
    }
}

//void    matching(Client & request) {
//    if (request.matchState == CRLF) {
//        while (request.position < request.buffSize && request.boundBuf.size() <= 2)
//            request.boundBuf += request.buf[request.position++];
//        if (request.boundBuf.size() == 2) {
//            if (request.boundBuf == "\r\n")
//                request.boundState = BOD;
//            request.boundBuf.clear();
//            request.matchState = NON;
//        }
//    }
//    if (request.matchState == BOUND) {
//        while (request.position < request.buffSize && request.boundBuf.size() <= request.boundary.size() + 4)
//            request.boundBuf += request.buf[request.position++];
//        if (request.boundBuf.size() == request.boundary.size() + 4) {
//            if (request.boundBuf == "--" + request.boundary + "--")
//                throw 201;
//            else if (request.boundBuf == "--" + request.boundary + "\r\n")
//                request.boundState = HEAD;
//            else
//                request.outfile << request.boundBuf;
//            request.boundBuf.clear();
//            request.matchState = NON;
//        }
//    }
//}

void    unBound(Client & request) {
    std::stringstream   ss;
    std::string         buf(request.buf), contentType;

    ss << request.boundBuf;
    request.boundBuf.clear();
    ss << buf.substr(request.position);
    buf.clear();
    while (getline(ss, buf)) {
        if (buf[buf.size() - 1] == '\r')
            buf = buf.substr(0, buf.size() - 1);
        if (buf == "--" + request.boundary + "--")
            throw 200;
        if (ss.eof()) {
            request.boundBuf = buf;
            return;
        }
        if (request.boundState != HEAD && buf == "--" + request.boundary) {
            request.boundState = HEAD;
            request.outfile->close();
        }
        else if (request.boundState == HEAD) {
            if (buf.empty())
                request.boundState = BOD;
            if (Tools::toLower(buf.substr(0, buf.find(':'))) == "content-type")
                contentType = buf.substr(buf.find(':') + 1);
        }
        else if (request.boundState == BOD) {
            if (!request.outfile->is_open()) {
                std::string extension, uploadPath;

                uploadPath = request.location->second.root + request.location->second.uplode.second;
                if (!contentType.empty())
                    extension = FileType::getExt(contentType);
                Tools::getAndCheckPath(uploadPath, extension);
                if (!contentType.empty())
                    request.outfile->open(uploadPath.c_str());
                else {
                    std::stringstream   ss;

                    ss << request.location->second.root + request.location->second.uplode.second + "/" << rand() << ".file";
                    extension = ss.str();
                    std::cout << ">>>>" << extension << std::endl;
                    request.outfile->open(extension.c_str());
                }
                contentType.clear();
            }
            *(request.outfile) << buf;
        }
    }
}

void    bodyParser(Client & request) {
    if (request.isBound && !request.isCgi)
        unBound(request);
	else if (request.headers["transfer-encoding"] == "chunked")
		unCh(request);
    else {
		size_t st = request.position;

		if (request.contentLength >= request.buffSize - st) {
			while (st < request.buffSize)
				*(request.outfile) << request.buf[st++];
			request.contentLength -= request.buffSize - request.position;
			request.position = 0;
			if (request.contentLength == 0)
				throw 201;
		}
		else
			throw 413;
    }
}

void	createOutfile(Client & request) {
	if (!request.isDir && request.location->second.cgi.first) {
		std::map<std::string, std::string>::iterator	it;
		std::string										extension;

		Tools::getExtension(request.path, extension);
		it = request.location->second.cgi.second.find(extension);
		if (it != request.location->second.cgi.second.end()) {
			std::stringstream ss;

			request.cgiScript = it->second;
			request.isCgi = true;
			ss << rand();
			request.cgiFileName = "temp/" + ss.str() + "_cgi_in.tmp";
			request.outfile->open(request.cgiFileName.c_str());
		}
	}
	if (!request.isCgi && request.location->second.uplode.first) {
		std::string extension, uploadPath;

		extension = FileType::getExt(request.headers["content-type"]);
		uploadPath = request.location->second.root + request.location->second.uplode.second;
		Tools::getAndCheckPath(uploadPath, extension);
		request.outfile->open(uploadPath.c_str());
	}
	if (!request.isCgi && !request.location->second.uplode.first)
		throw 403;
}

void	postHandler(Client & request) {
	if (!request.isBound && !request.outfile->is_open())
		createOutfile(request);
	if (request.chunkSize >= request.buffSize) {
		request.outfile->write(request.buf, request.buffSize);
		request.chunkSize -= request.buffSize;
		request.position = 2;
	}
	else
		bodyParser(request);
}
