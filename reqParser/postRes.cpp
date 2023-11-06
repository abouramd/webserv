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

void    bodyParser(Client & request) {
	if (request.headers["Transfer-Encoding"] == "chunked") {
		unCh(request);
	}
    else {
		size_t st = request.position;

		if (request.contentLength >= request.buffSize - st) {
			while (st < request.buffSize)
				*(request.outfile) << request.buf[st++];
			request.contentLength -= request.buffSize - request.position;
			request.position = 0;
			if (request.contentLength == 0)
				throw 200;
		}
		else
			throw 413;
    }
}

void	postHandler(Client & request) {
	if (!request.outfile->is_open()) {
		if (request.isCgi) {
			std::stringstream	ss;

			ss << rand();
			request.cgiFileName = "_tmp/" + ss.str() + "_cgi_in.tmp";
			request.outfile->open(request.cgiFileName.c_str());
		}
		else if (request.location->second.uplode.first) {
			std::string		extension , uploadPath;

			extension = FileType::getExt(request.headers["Content-Type"]);
			uploadPath = request.location->second.root + request.location->second.uplode.second;
			Tools::getAndCheckPath(uploadPath, extension);
			request.outfile->open(uploadPath.c_str());
		}
		else
			throw 405;
		if (!request.outfile->is_open())
			throw 501;
	}
	if (request.chunkSize >= request.buffSize) {
		std::cout << "mmmm" << std::endl;

		request.outfile->write(request.buf, request.buffSize);
		request.chunkSize -= request.buffSize;
		request.position = 2;
	}
	else
		bodyParser(request);
}