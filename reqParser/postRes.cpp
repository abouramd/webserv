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

		Tools::getExtension(request.fullPath, extension);
        std::cout << BLUE << "EXT: " << extension << std::endl;
		it = request.location->second.cgi.second.find(extension);
        std::cout << request.location->second.cgi.second.find(extension)->second << std::endl;
		if (it != request.location->second.cgi.second.end()) {
			std::stringstream ss;

			request.cgiScript = it->second;
			request.isCgi = true;
			ss << rand();
			request.cgiFileName = "temp/" + ss.str() + "_cgi_in.tmp";
			request.outfile->open(request.cgiFileName.c_str());
		}
	}
    if (!request.isCgi && request.isBound) {
        std::string buf(request.buf);

        buf = buf.substr(request.position, request.boundary.size() + 2);
        if (buf != "--" + request.boundary)
            throw 400;
        request.position += request.boundary.size() + 4;
        request.beenThere = true;
        return;
    }
	else if (!request.isCgi && request.location->second.uplode.first) {
		std::string extension, uploadPath;

		extension = FileType::getExt(request.headers["content-type"]);
		uploadPath = request.location->second.root + request.location->second.uplode.second;
		Tools::getAndCheckPath(uploadPath, extension);
		request.outfile->open(uploadPath.c_str());
	}
	if (!request.isCgi && !request.location->second.uplode.first)
		throw 403;
    request.beenThere = true;
}

void	postHandler(Client & request) {
	if (!request.beenThere && !request.outfile->is_open())
		createOutfile(request);
	if (request.chunkSize >= request.buffSize) {
		request.outfile->write(request.buf, request.buffSize);
		request.chunkSize -= request.buffSize;
		request.position = 2;
	}
	else
		bodyParser(request);
}
