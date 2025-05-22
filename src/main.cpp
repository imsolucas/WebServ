# include <iostream>
# include <ostream>
# include <cstring>

# include "colors.h"
# include "utils.hpp"
# include "Http.h"
# include "WebServer.hpp"

using std::string;
using std::map;
using std::ostringstream;
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
		const char *stream =
		"GET /index.html HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9\r\n"
		"Connection: keep-alive\r\n"
		"\r\n";

		HttpRequest request = deserialize(stream);

		cout << request;
	}
	cout << "\n\n";
	// POST request
	{
		const char *stream =
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

		cout << request;
	}
	cout << "\n\n";
	// HTTP response with no body
	{
		map<string, string> headers;
		headers["Content-Length"] = "0";
		headers["Connection"] = "close";
		HttpResponse response =
		{
			"HTTP/1.1",
			NO_CONTENT,
			"No Content",
			headers,
			""
		};

		string str = serialize(response);
		char *stream = new char[str.size() + 1];
		std::strcpy(stream, str.c_str());
		cout << stream;
		delete[] stream;
	}
	cout << "\n\n";
	// HTTP response with body
	{
		string body = "Hello World !";
		map<string, string> headers;
		headers["Content-Type"] = "text/plain; charset=UTF-8";
		headers["Content-Length"] = utils::toString(body.size());
		headers["Connection"] = "close";
		HttpResponse response =
		{
			"HTTP/1.1",
			NO_CONTENT,
			"No Content",
			headers,
			body
		};

		string str = serialize(response);
		char *stream = new char[str.size() + 1];
		std::strcpy(stream, str.c_str());
		cout << stream;
		delete[] stream;
	}
}
