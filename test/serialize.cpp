# include <ostream>
# include <cstring>

# include "utils.hpp"
# include "Http.hpp"
# include "test.hpp"

using std::string;
using std::map;

static bool test_no_body();
static bool test_with_body();

void test_serialize(TestSuite &t)
{
	t.addTest(test_no_body);
	t.addTest(test_with_body);
}

bool test_no_body()
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

bool test_with_body()
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
