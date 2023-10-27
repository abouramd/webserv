#include "reqParse.hpp"

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
        request.outfile.write(request.buf, request.chunkSize);
        request.position = request.chunkSize + 2;
        request.chunkSize = 0;
        if (request.contentLength <  static_cast<int>(request.chunkSize))
            throw 413;
        request.contentLength -= request.chunkSize;
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
        if (request.chunkSize == 0) {
            request.state = DONE;
            break;
        }
        if (st >= request.buffSize) {
            request.position = st - request.buffSize;
            break;
        }
        while (request.chunkSize && st < request.buffSize) {
            request.outfile << request.buf[st];
            request.chunkSize--;
            request.contentLength--;
            st++;
        }
        if (request.contentLength < 0)
            throw 413;
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
    if (request.headers["Transfer-Encoding"] == "chunked")
        unCh(request);
    else {
        size_t st = request.position;

        if (request.contentLength <  static_cast<int>(request.buffSize - st)) {
			while (request.contentLength > 0)
				request.outfile << request.buf[st++];
		}
		else {
			while (st < request.buffSize)
				request.outfile << request.buf[st++];
			request.contentLength -= request.buffSize - request.position;
			request.position = 0;
		}
    }
}