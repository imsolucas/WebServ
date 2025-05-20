# pragma once

# include <iostream>
# include <map>

enum MessageType
{
	REQUEST,
	RESPONSE
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

struct HttpHeader
{
	std::map<std::string, std::string> fields;
};

struct HttpMessage
{
	MessageType	type;
	std::string	startLine;
	HttpHeader	header;
	std::string	body;
};
