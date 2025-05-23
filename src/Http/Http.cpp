# include <iostream>
# include <sstream>

# include "utils.hpp"
# include "Http.h"

using std::string;
using std::ostream;
using std::istringstream;
using std::vector;
using std::map;
using std::cout;
using std::getline;

static HttpRequest parse(HttpMessage message);
static HttpMessage decode(string stream);

string serialize(const HttpResponse &response)
{
	string stream;

	stream = response.protocol + " ";
	stream += utils::toString(response.statusCode) + " ";
	stream += response.statusText + "\r\n";
	for (map<string, string>::const_iterator it = response.headers.begin();
		it != response.headers.end(); ++it)
		stream += (*it).first + ": " + (*it).second + "\r\n";
	stream += "\r\n";
	stream += response.body;

	return stream;
}

// assumes the byte stream is a valid HTTP request
HttpRequest deserialize(const string &stream)
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

HttpMessage decode(string stream)
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

ostream &operator << (ostream &os, const HttpRequest &r)
{
	string httpRequest;

	httpRequest = r.method + " " + r.requestTarget + " " + r.protocol + "\n";
	for (map<string, string>::const_iterator it = r.headers.begin();
		it != r.headers.end(); ++it)
		httpRequest += it->first + ": " + it->second + "\n";
	httpRequest += r.body;
	os << httpRequest;

	return os;
}

ostream &operator << (ostream &os, const HttpResponse &r)
{
	(void)r;
	return os;
}

bool HttpRequest::operator == (const HttpRequest &rhs) const
{
	if (method != rhs.method) return false;
	if (requestTarget != rhs.requestTarget) return false;
	if (protocol != rhs.protocol) return false;
	if (headers != rhs.headers) return false;
	if (body != rhs.body) return false;
	return true;
}

bool HttpResponse::operator == (const HttpResponse &rhs) const
{
	if (protocol != rhs.protocol) return false;
	if (statusCode != rhs.statusCode) return false;
	if (statusText != rhs.statusText) return false;
	if (headers != rhs.headers) return false;
	if (body != rhs.body) return false;
	return true;
}
