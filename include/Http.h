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

enum HttpHeader
{
	HOST,
	CONTENT_LENTGTH,
	CONTENT_TYPE,
	ACCEPT,
	AUTHORIZATION,
	COOKIE,
	CONNECTION,
	DATE,
	SERVER,
	LOCATION,
	ALLOW,
	SET_COOKIE
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

struct HttpMessage
{
	MessageType	type;
	std::string	startLine;
	std::map<std::string, std::string> headers;
	std::string	body;
};

struct HttpRequest
{
	HttpMethod	method;
	std::string	URI;
	std::string	httpVersion;
	std::map<HttpHeader, std::string> headers;
	std::string	body;
};

struct HttpResponse
{
	std::string	httpVersion;
	StatusCode	status;
	std::string	reasonPhrase;
	std::map<HttpHeader, std::string> headers;
	std::string	body;
};

ByteStream	serialize(HttpResponse response);
HttpRequest	deserialize(ByteStream request);
