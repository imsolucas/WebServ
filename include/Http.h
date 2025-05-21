# pragma once

# include <iostream>
# include <map>
# include <vector>

typedef char * ByteStream;

enum MessageType
{
	REQUEST,
	RESPONSE
};

enum HttpMethod
{
	GET,
	POST,
	DELETE
};

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
	INTERNAL_SERVER_ERROR = 500,
	BAD_GATEWAY = 502
};

namespace HttpHeaders {
	const std::string HOST = "Host";
	const std::string CONTENT_LENGTH = "Content-Length";
	const std::string CONTENT_TYPE = "Content-Type";
	const std::string ACCEPT = "Accept";
	const std::string AUTHORIZATION = "Authorization";
	const std::string COOKIE = "Cookie";
	const std::string CONNECTION = "Connection";
	const std::string DATE = "Date";
	const std::string SERVER = "Server";
	const std::string LOCATION = "Location";
	const std::string ALLOW = "Allow";
	const std::string SET_COOKIE = "Set-Cookie";
}

struct HttpRequest
{
	HttpMethod	method;
	std::string	URI;
	std::string	httpVersion;
	std::map<std::string, std::string> headers;
	std::string	body;
};

struct HttpResponse
{
	std::string	httpVersion;
	StatusCode	status;
	std::string	reasonPhrase;
	std::map<std::string, std::string> headers;
	std::string	body;
};

ByteStream	serialize(HttpResponse response);
HttpRequest	deserialize(ByteStream stream);
