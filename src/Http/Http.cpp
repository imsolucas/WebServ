# include <iostream>
# include <sstream>

# include "Http.h"

using std::cout;
using std::getline;
using std::string;
using std::istringstream;

static HttpRequest parse(HttpMessage message);
static HttpMessage decode(ByteStream stream);

ByteStream serialize(HttpResponse response)
{
	(void)response;
	return NULL;
}

// assumes the byte stream is a valid HTTP request
HttpRequest deserialize(ByteStream stream)
{
	HttpMessage msg;
	HttpRequest req;

	msg = decode(stream);
	cout << msg.startLine;
	cout << msg.headers;
	cout << msg.body;
	req = parse(msg);

	return req;
}

HttpRequest parse(HttpMessage message)
{
	(void)message;
	HttpRequest req;
	return (req);
}

HttpMessage decode(ByteStream stream)
{
	istringstream iss(stream);
	string line;
	HttpMessage msg;

	// Read start line
	getline(iss, msg.startLine);

	// Read headers until an empty line
	while (getline(iss, line))
	{
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);
		if (line.empty())
			break;
		msg.headers += (line + "\n");
	}

	// Read the body
	while (getline(iss, line))
		msg.body += (line + "\n");

	return msg;
}
