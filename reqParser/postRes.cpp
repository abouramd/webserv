#include "Tools.hpp"

void bodyParser(Client &request) {
  if (request.isBound && !request.isCgi)
    unBound(request);
  else if (request.headers["transfer-encoding"] == "chunked")
    handleChunked(request);
  else {
    if (request.contentLength == 0)
      throw 200;

    int size = request.contentLength;
    if (request.contentLength > request.buffSize - request.position)
        size = request.buffSize - request.position;
    request.outfile->write(request.buf + request.position, size);
    request.outfile->flush();
    request.contentLength -= size;
    request.position += size;
    
    // *(request.outfile) << request.buf[request.position];
    // request.outfile->flush();
    // request.contentLength--;
    // request.position++;
    if (request.contentLength == 0) {
      if (request.position > request.buffSize)
        throw 413;
      throw 201;
    }
  }
}

void createOutfile(Client &request) {
  if (!request.isDir && request.location.second.cgi.first) {
    std::map<std::string, std::string>::iterator it;
    std::string extension;

 

    Tools::getExtension(request.fullPath, extension);

    // std::cout << BLUE << "EXT: " << extension << std::endl;
    it = request.location.second.cgi.second.find(extension);

    // std::cout << request.location.second.cgi.second.find(extension)->second << std::endl;
    if (it != request.location.second.cgi.second.end()) {

      std::stringstream ss;

      request.cgiScript = it->second;
      request.isCgi = true;
      ss << rand();
      request.cgiFileName = "temp/" + ss.str() + "_cgi_in.tmp";
      request.outfile->open(request.cgiFileName.c_str());

    }
   // std::cout << "heeeere" << std::endl;
   //  exit(4);
  }
  if (!request.isCgi && request.isBound) {
    std::string buf(request.buf);

    buf = buf.substr(request.position, request.boundary.size() + 2);
    if (buf != "--" + request.boundary)
      throw 400;
    request.position += request.boundary.size() + 4;
    request.beenThere = true;
    return;
  } else if (!request.isCgi && request.location.second.uplode.first) {
    std::string extension, uploadPath;

    extension = FileType::getExt(request.headers["content-type"]);
    uploadPath =
        request.location.second.root + request.location.second.uplode.second;
    Tools::getAndCheckPath(uploadPath, extension);
    request.outfile->open(uploadPath.c_str());
  }
  if (!request.isCgi && !request.location.second.uplode.first)
    throw 403;
  request.beenThere = true;
}

void postHandler(Client &request) {
  if (!request.beenThere && !request.outfile->is_open())
    createOutfile(request);
  bodyParser(request);
}
