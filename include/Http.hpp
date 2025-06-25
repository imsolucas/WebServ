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
	BAD_GATEWAY = 502,
	GATEWAY_TIMEOUT = 504
};

enum PathType
{
	R_FILE, // regular file
	DIRECTORY,
	OTHER,
	NOT_EXIST
};

namespace Http
{
	extern const std::map<StatusCode, std::string> statusText;
	extern const std::map<std::string, std::string> mimeType;

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

class Location;

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
std::ostream &operator << (std::ostream &os, const HttpRequest &r);

struct HttpResponse
{
	std::string	protocol;
	StatusCode	statusCode;
	std::string	statusText;
	std::map<std::string, std::string> headers;
	std::string	body;

	bool operator == (const HttpResponse &rhs) const;
};
std::ostream &operator << (std::ostream &os, const HttpResponse &r);

std::string	serialize(const HttpResponse &response);
HttpRequest	deserialize(const std::string &stream);
HttpRequest parse(const HttpMessage &message);
void parseHeader(const std::string &headerLine, std::map<std::string, std::string> &headers, bool &hostSeen);
HttpMessage decode(const std::string &stream);

HttpResponse serveFile(HttpRequest &request, const std::string &file);
const Location &matchURI(const std::string &URI, const std::vector<Location> &locations);
HttpResponse listDirectory(const Location &location, const std::string &directory);
std::string autoindex(const std::string &directory);
HttpResponse handleError(StatusCode code);

PathType getPathType(const std::string &path);
std::string getContentType(const std::string &file);
bool isCGI(const std::string &file);
