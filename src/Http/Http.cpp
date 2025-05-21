# include <iostream>
# include <sstream>

# include "utils.h"
# include "Http.h"

using std::string;
using std::istringstream;
using std::vector;
using std::cout;
using std::getline;

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
	req = parse(msg);

	return req;
}

HttpRequest parse(HttpMessage message)
{
	HttpRequest req;

	vector<string> vec = utils::split(message.startLine, ' ');
	req.method = vec[0];
	req.requestTarget = vec[1];
	req.protocol = vec[2];

	for (vector<string>::iterator it = message.headers.begin();
		it != message.headers.end(); ++it)
	{
		vec = utils::split(*it, ':');
		req.headers[vec[0]] = vec[1].substr(1);
	}

	req.body = message.body;

	return (req);
}

HttpMessage decode(ByteStream stream)
{
	istringstream iss(stream);
	string line;
	HttpMessage msg;

	// Read start line
	getline(iss, line);
	if (!line.empty() && line[line.length() - 1 == '\r'])
		line.erase(line.length() - 1);
	msg.startLine = line;

	// Read headers until an empty line
	while (getline(iss, line))
	{
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);
		if (line.empty())
			break;
		msg.headers.push_back(line);
	}

	// Read the body
	while (getline(iss, line))
		msg.body += (line + "\n");

	return msg;
}
