# include <ostream>
# include <cstring>

# include "utils.hpp"
# include "Http.h"
# include "test.hpp"

using std::string;
using std::map;

static bool test_serialize_no_body();
static bool test_serialize_with_body();
static bool test_deserialize_get();
static bool test_deserialize_post();

void test_http(TestSuite &t)
{
	t.addTest(test_serialize_no_body);
	t.addTest(test_serialize_with_body);
	t.addTest(test_deserialize_get);
	t.addTest(test_deserialize_post);
}

bool test_serialize_no_body()
{
	string message = "serialize HTTP response with no body";
	map<string, string> headers;
	headers["Content-Length"] = "0";
	headers["Connection"] = "close";
	HttpResponse response = {
		"HTTP/1.1",
		NO_CONTENT,
		"No Content",
		headers,
		""
	};
	string actual = serialize(response);

	string expected =
		"HTTP/1.1 204 No Content\r\n"
		"Connection: close\r\n"
		"Content-Length: 0\r\n"
		"\r\n";

	return assertEqual(message, actual, expected);
}

bool test_serialize_with_body()
{
	string message = "serialize HTTP response with body";
	string body = "Hello World !";
	map<string, string> headers;
	headers["Content-Type"] = "text/plain; charset=UTF-8";
	headers["Content-Length"] = utils::toString(body.size());
	headers["Connection"] = "close";
	HttpResponse response = {
		"HTTP/1.1",
		NO_CONTENT,
		"No Content",
		headers,
		body
	};
	string actual = serialize(response);

	string expected =
		"HTTP/1.1 204 No Content\r\n"
		"Connection: close\r\n"
		"Content-Length: 13\r\n"
		"Content-Type: text/plain; charset=UTF-8\r\n"
		"\r\n"
		"Hello World !";

	return assertEqual(message, actual, expected);
}

bool test_deserialize_get()
{
	string message = "deserialize GET actual";
	const char *stream =
		"GET /index.html HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9\r\n"
		"Connection: keep-alive\r\n"
		"\r\n";
	HttpRequest actual = deserialize(stream);

	map<string, string> headers;
	headers["host"] = "example.com";
	headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64)";
	headers["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9";
	headers["connection"] = "keep-alive";
	HttpRequest expected = {
		"GET",
		"/index.html",
		"HTTP/1.1",
		headers,
		""
	};

	return assertEqual(message, actual, expected);
}

bool test_deserialize_post()
{
	string message = "deserialize POST actual";
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
	HttpRequest actual = deserialize(stream);

	map<string, string> headers;
	headers["host"] = "example.com";
	headers["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64)";
	headers["content-type"] = "application/json";
	headers["content-length"] = "34";
	headers["connection"] = "keep-alive";
	HttpRequest expected = {
		"POST",
		"/api/v1/resource",
		"HTTP/1.1",
		headers,
		"{\n"
		"    \"name\": \"John Doe\",\n"
		"    \"age\": 30\n"
		"}"
	};

	return assertEqual(message, actual, expected);
}
