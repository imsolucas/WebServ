# include "utils.hpp"
# include "Http.h"
# include "test.hpp"

// using std::string;

// static bool test_staticFileReadable();
// static bool test_staticFileNotReadable();
// static bool test_cgiFileExecutable();
// static bool test_cgiFilePermissionDenied();
// static bool test_nonExistentFile();

// void test_buildResponse(TestSuite &t)
// {
// 	t.addTest(test_staticFileReadable);
// 	t.addTest(test_staticFileNotReadable);
// 	t.addTest(test_cgiFileExecutable);
// 	t.addTest(test_cgiFilePermissionDenied);
// 	t.addTest(test_nonExistentFile);
// }

// bool test_staticFileReadable()
// {
// 	string message = "Build response for readable static file";

// 	// Mock request and file path
// 	HttpRequest request;
// 	request.method = Http::GET;
// 	string file = "/var/www/html/index.html";

// 	// Mock utils::readFile() to return file content
// 	utils::mockReadFile(file, "<html><body>Hello World!</body></html>");

// 	// Mock access() to simulate file readability
// 	utils::mockAccess(file, R_OK);

// 	// Call serveFile()
// 	HttpResponse actual = serveFile(request, file);

// 	// Expected response
// 	HttpResponse expected;
// 	expected.protocol = "HTTP/1.1";
// 	expected.statusCode = OK;
// 	expected.statusText = "OK";
// 	expected.headers[Http::CONTENT_TYPE] = "text/html";
// 	expected.headers[Http::CONTENT_LENGTH] = "37";
// 	expected.headers[Http::SERVER] = "webserv";
// 	expected.headers[Http::DATE] = utils::genTimeStamp();
// 	expected.body = "<html><body>Hello World!</body></html>";

// 	// Validate response
// 	return assertEqual(message, actual, expected);
// }

// bool test_staticFileNotReadable()
// {
// 	std::string message = "Build response for static file not readable";

// 	// Mock request and file path
// 	HttpRequest request;
// 	request.method = Http::GET;
// 	std::string file = "/var/www/html/index.html";

// 	// Mock access() to simulate file not readable
// 	utils::mockAccess(file, -1);

// 	// Call serveFile()
// 	HttpResponse actual = serveFile(request, file);

// 	// Expected response
// 	HttpResponse expected = handleError(FORBIDDEN);

// 	// Validate response
// 	return assertEqual(message, actual, expected);
// }

// bool test_cgiFileExecutable()
// {
// 	std::string message = "Build response for executable CGI file";

// 	// Mock request and file path
// 	HttpRequest request;
// 	request.method = Http::GET;
// 	std::string file = "/var/www/cgi-bin/script.cgi";

// 	// Mock CGIHandler to simulate successful execution
// 	CGIHandler::mockExecute(file, OK, "CGI Output");

// 	// Mock access() to simulate file executable
// 	utils::mockAccess(file, X_OK);

// 	// Call serveFile()
// 	HttpResponse actual = serveFile(request, file);

// 	// Expected response
// 	HttpResponse expected;
// 	expected.protocol = "HTTP/1.1";
// 	expected.statusCode = OK;
// 	expected.statusText = "OK";
// 	expected.headers[Http::CONTENT_LENGTH] = "10";
// 	expected.headers[Http::SERVER] = "webserv";
// 	expected.headers[Http::DATE] = utils::genTimeStamp();
// 	expected.body = "CGI Output";

// 	// Validate response
// 	return assertEqual(message, actual, expected);
// }

// bool test_cgiFilePermissionDenied()
// {
// 	std::string message = "Build response for CGI file with permission denied";

// 	// Mock request and file path
// 	HttpRequest request;
// 	request.method = Http::GET;
// 	std::string file = "/var/www/cgi-bin/script.cgi";

// 	// Mock access() to simulate file not executable
// 	utils::mockAccess(file, -1);

// 	// Call serveFile()
// 	HttpResponse actual = serveFile(request, file);

// 	// Expected response
// 	HttpResponse expected = handleError(FORBIDDEN);

// 	// Validate response
// 	return assertEqual(message, actual, expected);
// }

// bool test_nonExistentFile()
// {
// 	std::string message = "Build response for non-existent file";

// 	// Mock request and file path
// 	HttpRequest request;
// 	request.method = Http::GET;
// 	std::string file = "/var/www/html/missing.html";

// 	// Mock access() to simulate file not found
// 	utils::mockAccess(file, -1);

// 	// Call serveFile()
// 	HttpResponse actual = serveFile(request, file);

// 	// Expected response
// 	HttpResponse expected = handleError(NOT_FOUND);

// 	// Validate response
// 	return assertEqual(message, actual, expected);
// }
