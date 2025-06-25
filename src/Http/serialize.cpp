# include <iostream>
# include <sstream>
# include <cstdlib>

# include "utils.hpp"
# include "Http.hpp"

using std::runtime_error;
using std::vector;
using std::map;
using std::pair;
using std::istringstream;
using std::ostream;
using std::string;
using std::cout;
using std::getline;

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

HttpRequest parse(const HttpMessage &message)
{
	HttpRequest req;

	// cout << "start line: " << message.startLine << "\n";
	// for (vector<string>::const_iterator it = message.headers.begin();
	// 	it != message.headers.end(); ++it)
	// {
	// 	cout << "header: " << *it + "\n";
	// }
	// cout << "body: " << message.body << "\n";

	vector<string> vec = utils::split(message.startLine, ' ');
	if (vec.size() != 3)
		throw runtime_error("BAD REQUEST: invalid start line");
	req.method = vec[0];
	if (req.method != "GET" && req.method != "POST"
		&& req.method != "DELETE")
		throw runtime_error("BAD REQUEST: invalid method");
	req.requestTarget = vec[1];
	if (req.requestTarget.find('\r') != string::npos
		|| req.requestTarget.find('\n') != string::npos)
		throw runtime_error("BAD REQUEST: invalid request target");
	req.protocol = vec[2];
	if (req.protocol != "HTTP/1.1")
		throw runtime_error("BAD REQUEST: invalid protocol");

	bool hostSeen = false;
	for (vector<string>::const_iterator it = message.headers.begin();
		it != message.headers.end(); ++it)
	{
		parseHeader(*it, req.headers, hostSeen);
	}
	if (req.headers.find("host") == req.headers.end())
		throw runtime_error("BAD REQUEST: Host header missing");

	req.body = message.body;
	if (req.method == "GET" && !req.body.empty())
		throw runtime_error("BAD REQUEST: invalid body");
	if (req.headers.find("content-length") != req.headers.end())
	{
		if (req.headers.find("transfer-encoding") != req.headers.end())
			throw runtime_error("BAD REQUEST: conflicting headers");
		if (!utils::isNum(req.headers["content-length"]))
			throw runtime_error("BAD REQUEST: invalid content length");
		if (req.body.size() != (size_t)atoi(req.headers["content-length"].c_str()))
			throw runtime_error("BAD REQUEST: wrong content length");
	}

	return (req);
}

void parseHeader(const string& headerLine, map<string, string>& headers, bool& hostSeen) {
	vector<string> vec = utils::splitFirst(headerLine, ':');
	if (vec.size() != 2)
		throw runtime_error("BAD REQUEST: invalid header format");

	vec[0] = utils::toLower(vec[0]);
	if (!utils::isPrint(vec[0]) || vec[0].find(' ') != string::npos
		|| vec[0].find('\r') != string::npos
		|| vec[0].find('\n') != string::npos)
		throw runtime_error("BAD REQUEST: invalid header type");

	if (vec[0] == "host")
	{
		if (hostSeen)
			throw runtime_error("BAD REQUEST: duplicate host header");
		hostSeen = true;
	}

	string value = utils::trim(vec[1], " ");
	if (!utils::isPrint(value) || value.find("\r\n") != string::npos)
		throw runtime_error("BAD REQUEST: invalid header content");

	headers[vec[0]] = value;
}

// TODO: merge decode() into parse()
HttpMessage decode(const string &stream)
{
	istringstream iss(stream);
	string line;
	HttpMessage msg;

	// Read start line
	getline(iss, line);
	if (line.empty() || line == "\r")
		throw runtime_error("BAD REQUEST: empty start line");
	if (line[line.length() - 1 == '\r'])
		line.erase(line.length() - 1);
	msg.startLine = line;

	// Read headers until an empty line
	bool foundEmptyLine = false;
	while (getline(iss, line))
	{
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);
		if (line.empty())
		{
			foundEmptyLine = true;
			break;
		}
		msg.headers.push_back(line);
	}
	if (!foundEmptyLine)
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
