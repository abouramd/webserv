#include "reqParse.hpp"

std::string generateResponse(int status) {
	std::string response;

	switch (status) {
		case 200:
			response = "HTTP/1.1 200 OK\r\n"
					   "Content-Type: text/plain\r\n"
					   "\r\n"
					   "Success!";
			break;
		case 404:
			response = "HTTP/1.1 404 Not Found\r\n"
					   "Content-Type: text/plain\r\n"
					   "\r\n"
					   "Page not found!";
			break;
		case 500:
			response = "HTTP/1.1 500 Internal Server Error\r\n"
					   "Content-Type: text/plain\r\n"
					   "\r\n"
					   "Internal server error!";
			break;
		default:
			response = "HTTP/1.1 400 Bad Request\r\n"
					   "Content-Type: text/plain\r\n"
					   "\r\n"
					   "Bad request!";
			break;
	}

	return response;
}