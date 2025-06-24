# include <iostream>
# include <sstream>
# include <cstdlib>

# include "utils.hpp"
# include "Http.h"

using std::runtime_error;
using std::vector;
using std::map;
using std::pair;
using std::istringstream;
using std::ostream;
using std::string;
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
	if (vec.size() != 3)
		throw runtime_error("BAD REQUEST: invalid start line\n");
	req.method = vec[0];
	if (req.method != "GET" && req.method != "POST"
		&& req.method != "DELETE")
		throw runtime_error("BAD REQUEST: invalid method\n");
	req.requestTarget = vec[1];
	if (req.requestTarget.find('\r') != string::npos
		|| req.requestTarget.find('\n') != string::npos)
		throw runtime_error("BAD REQUEST: invalid request target\n");
	req.protocol = vec[2];
	if (req.protocol != "HTTP/1.1")
		throw runtime_error("BAD REQUEST: invalid protocol\n");

	for (vector<string>::iterator it = message.headers.begin();
		it != message.headers.end(); ++it)
	{
		vec = utils::splitFirst(*it, ':');
		vec[0] = utils::toLower(vec[0]); // RFC 9110: field names are case-insensitive
		if (!utils::isPrint(vec[0]) || vec[0].find(' ') != string::npos
			|| vec[0].find('\r') != string::npos
			|| vec[0].find('\n') != string::npos)
			throw runtime_error("BAD REQUEST: invalid header type\n");
		req.headers[vec[0]] = utils::trim(vec[1], " ");
		if (!utils::isPrint(req.headers[vec[0]])
			|| req.headers[vec[0]].find("\r\n") != string::npos)
			throw runtime_error("BAD REQUEST: invalid header content\n");
	}
	if (req.headers.find("host") == req.headers.end())
		throw runtime_error("BAD REQUEST: Host header missing\n");

	req.body = message.body;
	if (req.method == "GET" && !req.body.empty())
		throw runtime_error("BAD REQUEST: invalid body\n");
	if (req.headers.find("content-length") != req.headers.end())
	{
		if (req.headers.find("transfer-encoding") != req.headers.end())
			throw runtime_error("BAD REQUEST: conflicting headers\n");
		if (!utils::isNum(req.headers["content-length"]))
			throw runtime_error("BAD REQUEST: invalid content length\n");
		if (req.body.size() != (size_t)atoi(req.headers["content-length"].c_str()))
			throw runtime_error("BAD REQUEST: wrong content length\n");
	}

	return (req);
}

HttpMessage decode(string stream)
{
	istringstream iss(stream);
	string line;
	HttpMessage msg;

	// Read start line
	getline(iss, line);
	if (line.empty())
		throw runtime_error("BAD REQUEST: empty start line");
	if (line[line.length() - 1 == '\r'])
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
	if (!line.empty())
		throw runtime_error("BAD REQUEST: missing empty line");

	// Read the body
	while (getline(iss, line))
		msg.body += (line + "\n");
	if (!msg.body.empty())
		msg.body = msg.body.substr(0, msg.body.length() - 1);

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
	string httpResponse;

	httpResponse = r.protocol + " " + utils::toString(r.statusCode) + " " + r.statusText + "\n";
	for (map<string, string>::const_iterator it = r.headers.begin();
		it != r.headers.end(); ++it)
		httpResponse += it->first + ": " + it->second + "\n";
	httpResponse += "\n" + r.body;
	os << httpResponse;
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
