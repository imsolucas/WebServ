#include "CGIHandler.hpp"
#include "Http.h"
# include <iostream>

int main()
{
	const char *chunkedRequestBody =
    "7\r\n"
    "name=J\r\n"
    "6\r\n"
    "hn&age\r\n"
    "3\r\n"
    "=36\r\n"
    "0\r\n"
    "\r\n";

	HttpRequest req;
	req.headers["transfer-encoding"] = "chunked";
	req.body = chunkedRequestBody;

	CGIHandler handler(req); // assuming your CGIHandler takes a Request object
	handler._unchunkBody();

	std::cout << req.body << std::endl;  // Should output: name=John&age=36

}
