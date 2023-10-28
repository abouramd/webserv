#include "reqParse.hpp"
std::string inHtml( std::string const &title, std::string const & message) {
    std::string htmlMessage = "<!DOCTYPE html>\n"
                              "<html>\n"
                              "<head>\n"
                              "    <title>" + title + "</title>\n"
                              "</head>\n"
                              "<body>\n"
                              "    <h1>" + title + "</h1>\n"
                              "    <p>" + message + "</p>\n"
                              "</body>\n"
                              "</html>";
    return htmlMessage;
}
std::string generateResponse(int status) {
	std::string response;
    std::string body;
    std::ifstream   index("index.html");

    switch (status) {
        case 200:
            getline(index, body, '\0');
			response = "HTTP/1.1 200 OK\r\n"
					   "Content-Type: text/html\r\n"
                       "Content-Length: " + std::to_string(body.size()) + "\r\n"
					   "\r\n" + body;
			break;
        case 201:
            body = inHtml("201 Created", "The request has been fulfilled and a new resource has been created.");
            response = "HTTP/1.1 201 Created\r\n"
                       "Content-Type: text/html\r\n"
                       "Location: /file.\r\n"
                       "Content-Length: " + std::to_string(body.size()) + "\r\n"
                       "\r\n" + body;
            break;
		case 404:
			response = "HTTP/1.1 404 Not Found\r\n"
					   "Content-Type: text/plain\r\n"
                       "Content-Length: 15\r\n"
					   "\r\n"
					   "Page not found!";
			break;
		case 500:
			response = "HTTP/1.1 500 Internal Server Error\r\n"
					   "Content-Type: text/plain\r\n"
                       "Content-Length: 22\r\n"
					   "\r\n"
					   "Internal server error!";
			break;
		default:
			response = "HTTP/1.1 " + std::to_string(status) + " Bad Request\r\n"
					   "Content-Type: text/plain\r\n"
                       "Content-Length: 44\r\n"
					   "\r\n"
					   "Bad request! or some other hta nzidhome hh !";
			break;
	}
    index.close();
	return response;
}