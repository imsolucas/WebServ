# include <iostream>

# include "colors.h"
# include "Http.h"
# include "WebServer.hpp"

using std::map;
using std::string;
using std::cout;
using std::cerr;

int main(int argc, char *argv[])
{
	(void)argv;
	if (argc != 2)
	{
		cerr << RED + "Usage: ./webserv [configuration file]\n";
		return 1;
	}

	// GET request
	{
		ByteStream stream =
		"GET /index.html HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9\r\n"
		"Connection: keep-alive\r\n"
		"\r\n";

		HttpRequest request = deserialize(stream);

		// cout << request.method << '\n';
		// cout << request.requestTarget << '\n';
		// cout << request.protocol << '\n';
		// for (map<string, string>::const_iterator it = request.requestHeaders.begin(); it != request.requestHeaders.end(); ++it)
		// 	cout << (*it).first + ": " + (*it).second + '\n';
		// cout << request.body << '\n';
	}
	cout << "\n\n";
	// POST request
	{
		ByteStream stream =
			"POST /api/v1/resource HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n"
			"Content-Type: application/json\r\n"
			"Content-Length: 34\r\n"
			"Connection: keep-alive\r\n"
			"\r\n"
			"{\n"
			"    \"name\": \"John Doe\",\n"
			"    \"age\": 30\n"
			"}";

		HttpRequest request = deserialize(stream);

		// cout << request.method << '\n';
		// cout << request.requestTarget << '\n';
		// cout << request.protocol << '\n';
		// for (map<string, string>::const_iterator it = request.requestHeaders.begin(); it != request.requestHeaders.end(); ++it)
		// 	cout << (*it).first + ": " + (*it).second + '\n';
		// cout << request.body << '\n';
	}
}
