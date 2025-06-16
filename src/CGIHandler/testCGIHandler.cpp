// #include "CGIHandler.hpp"
// #include "Http.h"
// #include <iostream>

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

// int testExecute()
// {
// 	try {
// 		const char *request =
// 		"GET cgi-bin/infinite_loop.py HTTP/1.1\r\n"
// 		"Host: localhost\r\n"
// 		"User-Agent: TestClient/1.0\r\n"
// 		"Accept: */*\r\n"
// 		"X-Custom-Header: custom_value\r\n"
// 		"\r\n";

// 	HttpRequest req = deserialize(request);
// 	CGIHandler handler(req);

// 	// (Optional) Set up handler with request/headers/body as needed
// 	// handler.setRequest(...);

// 	int statusCode = handler.execute();

// 	if (statusCode == 0)
// 	{
// 		std::string output = handler.getCGIOutput();
// 		std::cout << "CGI executed successfully:\n" << output << std::endl;
// 	}
// 	else
// 	{
// 		std::cerr << "CGI failed with status code: " << statusCode << std::endl;
// 	}
// 	} catch (const std::exception& e)
// 	{
// 		std::cerr << "Unhandled exception: " << e.what() << std::endl;
// 		return 1;
// 	}

// 	return 0;
// }
