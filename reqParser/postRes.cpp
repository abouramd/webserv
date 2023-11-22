#include "Tools.hpp"

void    bodyParser(Client & request) {
    if (request.isBound && !request.isCgi)
        unBound(request);
	else if (request.headers["transfer-encoding"] == "chunked")
		handleChunked(request);
    else {
		std::cout << request.isCgi << ",, ,," << request.contentLength << std::endl;
		  if (request.contentLength == 0)
			  throw 200;
        if (request.contentLength < request.buffSize - request.position) {
            std::remove(request.uploadFile.c_str());
            throw 413;
        }
        request.outfile->write(request.buf + request.position, request.buffSize - request.position);
        request.outfile->flush();
        request.contentLength -= request.buffSize - request.position;
        request.position = request.buffSize;
        if (request.contentLength == 0)
            throw 201;
    }
}

void	createOutfile(Client & request) {


	if (!request.isDir && request.location.second.cgi.first) {
		std::map<std::string, std::string>::iterator	it;
		std::string										extension;

		Tools::getExtension(request.fullPath, extension);
		it = request.location.second.cgi.second.find(extension);
		if (it != request.location.second.cgi.second.end()) {
			std::stringstream ss;

			request.cgiScript = it->second;
			request.isCgi = true;
			ss << rand();
			request.cgiFileName = ss.str() + "_cgi_in.tmp";
			request.outfile->open(request.cgiFileName.c_str());
		}
	}
    if (!request.isCgi && request.isBound) {
        request.beenThere = true;
        return;
    }
	else if (!request.isCgi && request.location.second.uplode.first) {
		std::string extension, uploadPath;

		extension = FileType::getExt(request.headers["content-type"]);
		uploadPath = request.location.second.root + request.location.second.uplode.second;
		Tools::getAndCheckPath(uploadPath, extension);
        request.uploadFile = uploadPath;
		request.outfile->open(uploadPath.c_str());
	}
	if (!request.isCgi && !request.location.second.uplode.first)
		throw 403;
    request.beenThere = true;
}

void	postHandler(Client & request) {
	if (!request.beenThere && !request.outfile->is_open())
		createOutfile(request);
    bodyParser(request);
}
