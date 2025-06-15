# pragma once

# include <string>
# include <ostream>
# include <map>
# include <vector>

enum StatusCode
{
	OK = 200,
	CREATED = 201,
	NO_CONTENT = 204,
	MOVED_PERMANENTLY = 301,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	CONTENT_TOO_LARGE = 413,
	INTERNAL_SERVER_ERROR = 500,
	BAD_GATEWAY = 502
};

namespace Http
{
	extern const std::map<StatusCode, std::string> statusText;
	
	const std::string GET = "GET";
	const std::string POST = "POST";
	const std::string DELETE = "DELETE";
	const std::string HOST = "Host";
	const std::string ACCEPT = "Accept";
	const std::string AUTHORIZATION = "Authorization";
	const std::string COOKIE = "Cookie";
	const std::string CONNECTION = "Connection";
	const std::string DATE = "Date";
	const std::string SERVER = "Server";
	const std::string LOCATION = "Location";
	const std::string ALLOW = "Allow";
	const std::string SET_COOKIE = "Set-Cookie";
	const std::string CONTENT_LENGTH = "Content-Length";
	const std::string CONTENT_TYPE = "Content-Type";
	const std::string CONTENT_RANGE = "Content-Range";
	const std::string CONTENT_ENCODING = "Content-Encoding";
	const std::string CONTENT_LOCATION = "Content-Location";
	const std::string CONTENT_LANGUAGE = "Content-Language";
}

struct HttpMessage
{
	std::string startLine;
	std::vector<std::string> headers;
	std::string body;
};

struct HttpRequest
{
	std::string	method;
	std::string	requestTarget;
	std::string	protocol;
	std::map<std::string, std::string> headers;
	std::string	body;

	bool operator == (const HttpRequest &rhs) const;
};

struct HttpResponse
{
	std::string	protocol;
	StatusCode	statusCode;
	std::string	statusText;
	std::map<std::string, std::string> headers;
	std::string	body;

	bool operator == (const HttpResponse &rhs) const;
};

std::string	serialize(const HttpResponse &response);
HttpRequest	deserialize(const std::string &stream);

std::ostream &operator << (std::ostream &os, const HttpRequest &r);
std::ostream &operator << (std::ostream &os, const HttpResponse &r);
