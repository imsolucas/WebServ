# include <ostream>
# include <cstring>

# include "utils.hpp"
# include "Http.h"
# include "test.hpp"

using std::exception;
using std::string;
using std::map;

static bool test_valid_get();
static bool test_valid_post();
static bool test_missing_host_header();
static bool test_wrong_content_length();
static bool test_invalid_method();
static bool test_invalid_protocol();
static bool test_empty_start_line();
static bool test_invalid_start_line_format();
static bool test_header_without_colon();
static bool test_duplicate_host_header();
static bool test_conflicting_content_length_and_transfer_encoding();
static bool test_invalid_content_length_value();
static bool test_missing_empty_line_after_headers();

void test_deserialize(TestSuite &t)
{
	t.addTest(test_valid_get);
	t.addTest(test_valid_post);
	t.addTest(test_missing_host_header);
	t.addTest(test_wrong_content_length);
	t.addTest(test_invalid_method);
	t.addTest(test_invalid_protocol);
	t.addTest(test_empty_start_line);
	t.addTest(test_invalid_start_line_format);
	t.addTest(test_header_without_colon);
	t.addTest(test_duplicate_host_header);
	t.addTest(test_conflicting_content_length_and_transfer_encoding);
	t.addTest(test_invalid_content_length_value);
	t.addTest(test_missing_empty_line_after_headers);
}

bool test_valid_get()
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

bool test_valid_post()
{
	string message = "deserialize POST actual";
	const char *stream =
		"POST /api/v1/resource HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: 41\r\n"
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
	headers["content-length"] = "41";
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

bool test_missing_host_header()
{
	string message = "deserialize missing Host header";
	const char *stream =
		"GET / HTTP/1.1\r\n"
		"Connection: close\r\n"
		"\r\n";
	try {
		HttpRequest actual = deserialize(stream);
	} catch (const exception &e) {
		return assertEqual(message, string(e.what()), string("BAD REQUEST: Host header missing"));
	}
	return false; // Should not reach here
}

bool test_wrong_content_length()
{
	string message = "deserialize wrong content length";
	const char *stream =
		"POST /api HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Content-Length: 10\r\n"
		"\r\n"
		"short";
	try {
		HttpRequest actual = deserialize(stream);
	} catch (const exception &e) {
		return assertEqual(message, string(e.what()), string("BAD REQUEST: wrong content length"));
	}
	return false; // Should not reach here
}

bool test_invalid_method()
{
	string message = "deserialize invalid method";
	const char *stream =
		"FOO / HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"\r\n";
	try {
		HttpRequest actual = deserialize(stream);
	} catch (const exception &e) {
		return assertEqual(message, string(e.what()), string("BAD REQUEST: invalid method"));
	}
	return false; // Should not reach here
}

bool test_invalid_protocol()
{
	string message = "deserialize invalid protocol";
	const char *stream =
		"GET / HTTP/2.0\r\n"
		"Host: example.com\r\n"
		"\r\n";
	try {
		HttpRequest actual = deserialize(stream);
	} catch (const exception &e) {
		return assertEqual(message, string(e.what()), string("BAD REQUEST: invalid protocol"));
	}
	return false; // Should not reach here
}

bool test_empty_start_line()
{
	string message = "deserialize empty start line";
	const char *stream = "\r\nHost: example.com\r\n\r\n";
	try {
		HttpRequest actual = deserialize(stream);
	} catch (const exception &e) {
		return assertEqual(message, string(e.what()), string("BAD REQUEST: empty start line"));
	}
	return false;
}

bool test_invalid_start_line_format()
{
	string message = "deserialize invalid start line format";
	const char *stream = "GET /index.html\r\nHost: example.com\r\n\r\n";
	try {
		HttpRequest actual = deserialize(stream);
	} catch (const exception &e) {
		return assertEqual(message, string(e.what()), string("BAD REQUEST: invalid start line"));
	}
	return false;
}

bool test_header_without_colon()
{
	string message = "deserialize header without colon";
	const char *stream =
		"GET / HTTP/1.1\r\n"
		"Host example.com\r\n"
		"\r\n";
	try {
		HttpRequest actual = deserialize(stream);
	} catch (const exception &e) {
		return assertEqual(message, string(e.what()), string("BAD REQUEST: invalid header format"));
	}
	return false;
}

bool test_duplicate_host_header()
{
	string message = "deserialize duplicate Host header";
	const char *stream =
		"GET / HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Host: duplicate.com\r\n"
		"\r\n";
	try {
		HttpRequest actual = deserialize(stream);
	} catch (const exception &e) {
		// Depending on your implementation, you may want to check for duplicate header error
		return assertEqual(message, string(e.what()), string("BAD REQUEST: duplicate host header"));
	}
	return false;
}

bool test_conflicting_content_length_and_transfer_encoding()
{
	string message = "deserialize conflicting Content-Length and Transfer-Encoding";
	const char *stream =
		"POST / HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Content-Length: 5\r\n"
		"Transfer-Encoding: chunked\r\n"
		"\r\n"
		"abcde";
	try {
		HttpRequest actual = deserialize(stream);
	} catch (const exception &e) {
		return assertEqual(message, string(e.what()), string("BAD REQUEST: conflicting headers"));
	}
	return false;
}

bool test_invalid_content_length_value()
{
	string message = "deserialize invalid Content-Length value";
	const char *stream =
		"POST / HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Content-Length: abc\r\n"
		"\r\n"
		"abc";
	try {
		HttpRequest actual = deserialize(stream);
	} catch (const exception &e) {
		return assertEqual(message, string(e.what()), string("BAD REQUEST: invalid content length"));
	}
	return false;
}

bool test_missing_empty_line_after_headers()
{
	string message = "deserialize missing empty line after headers";
	const char *stream =
		"GET / HTTP/1.1\r\n"
		"Host: example.com\r\n";
	try {
		HttpRequest actual = deserialize(stream);
	} catch (const exception &e) {
		return assertEqual(message, string(e.what()), string("BAD REQUEST: missing empty line"));
	}
	return false;
}
