#include "CGIHandler.hpp"
#include "Http.h"
#include <iostream>

// void testCGIHandler(const string &stream)
// {
// 	// // GET request with query string
// 	// string stream =
// 	// 	"GET /test_cgi.py?name=Alice&age=25 HTTP/1.1\r\n"
// 	// 	"Host: example.com\r\n"
// 	// 	"User-Agent: TestClient/1.0\r\n"
// 	// 	"Accept: */*\r\n"
// 	// 	"Connection: close\r\n"
// 	// 	"\r\n";

// 	// // POST request with form data in body
// 	// string stream =
// 	// 	"POST /test_cgi.py HTTP/1.1\r\n"
// 	// 	"Host: example.com\r\n"
// 	// 	"User-Agent: TestClient/1.0\r\n"
// 	// 	"Content-Type: application/x-www-form-urlencoded\r\n"
// 	// 	"Content-Length: 42\r\n"
// 	// 	"Connection: close\r\n"
// 	// 	"\r\n"
// 	// 	"name=Bob+Smith&email=bob.smith%40mail.com";

// 	HttpRequest request = deserialize(stream);

// 	CGIHandler cgi(request);
// 	int status = cgi.execute();
// 	string cgiOutput;
// 	if (status == 0)
// 	{
// 		cgiOutput = cgi.getCGIOutput();
// 		std::cout << "\nCGI Output: \n" << cgiOutput << std::endl;
// 	}
// }

// // int testUnchunkBody()
// // {
// // 	const char *chunkedRequestBody =
// // 		"7\r\n"
// // 		"name=Jo\r\n"
// // 		"6\r\n"
// // 		"hn&age\r\n"
// // 		"3\r\n"
// // 		"=36\r\n"
// // 		"0\r\n"
// // 		"\r\n";

// // 	HttpRequest req;
// // 	req.headers["transfer-encoding"] = "chunked";
// // 	req.body = chunkedRequestBody;

// // 	CGIHandler handler(req); // assuming your CGIHandler takes a Request object

// // 	try
// // 	{
// // 		handler._unchunkBody();
// // 	}
// // 	catch (const std::exception& e)
// // 	{
// // 		std::cout << e.what() << std::endl;
// // 	}

// // 	std::cout << req.body << std::endl;  // Should output: name=John&age=36
// // }

int main()
{
	try {
		const char *request =
		"GET /cgi-bin/hello.php HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"User-Agent: TestClient/1.0\r\n"
		"Accept: */*\r\n"
		"X-Custom-Header: custom_value\r\n"
		"\r\n";

	HttpRequest req = deserialize(request);
	CGIHandler handler(req, "public");

	// (Optional) Set up handler with request/headers/body as needed
	// handler.setRequest(...);

	int statusCode = handler.execute();

	if (statusCode == 200)
	{
		// Print raw CGI output (if needed for debug)
		std::string output = handler.getCGIOutput();
		std::cout << "CGI executed successfully:\n" << output << std::endl;

		// Print headers
		const std::map<std::string, std::string> &headers = handler.getCGIHeaders();
		std::cout << "Parsed CGI Headers:\n";
		for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		{
			std::cout << it->first << ": " << it->second << std::endl;
		}

		// Print body
		const std::string &body = handler.getCGIBody();
		std::cout << "\nCGI Body:\n" << body << std::endl;
	}
	else
	{
		std::cerr << "CGI failed with status code: " << statusCode << std::endl;
	}
	} catch (const std::exception& e)
	{
		std::cerr << "Unhandled exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
