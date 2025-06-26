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
using std::make_pair;

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
	istringstream iss(stream);
	string line;
	HttpRequest req;

	// Parse start line
	getline(iss, line);
	if (line.empty() || line == "\r")
		throw runtime_error("BAD REQUEST: empty start line");
	if (line[line.length() - 1] == '\r')
		line.erase(line.length() - 1);
	vector<string> vec = utils::split(line, ' ');
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

	// Parse headers
	bool foundEmptyLine = false;
	bool hostSeen = false;
	while (getline(iss, line))
	{
		pair<string, string> header = parseHeader(line);
		if (header.first == "" && header.second == "")
		{
			foundEmptyLine = true;
			break;
		}
		if (header.first == "host")
		{
			if (hostSeen)
				throw runtime_error("BAD REQUEST: duplicate host header");
			hostSeen = true;
		}
		req.headers.insert(header);
	}
	if (!foundEmptyLine)
		throw runtime_error("BAD REQUEST: missing empty line");
	if (req.headers.find("host") == req.headers.end())
		throw runtime_error("BAD REQUEST: Host header missing");

	if (req.method == "GET" && !req.body.empty())
		throw runtime_error("BAD REQUEST: invalid body");
	if (req.headers.find("content-length") != req.headers.end())
	{
		if (req.headers.find("transfer-encoding") != req.headers.end())
			throw runtime_error("BAD REQUEST: conflicting headers");
		if (!utils::isNum(req.headers["content-length"]))
			throw runtime_error("BAD REQUEST: invalid content length");

		size_t contentLength = (size_t)atoi(req.headers["content-length"].c_str());
		string body(contentLength, '\0');
		iss.read(&body[0], contentLength);
		size_t bytesRead = iss.gcount();
		if (bytesRead != contentLength)
			throw runtime_error("BAD REQUEST: wrong content length");
		req.body = body.substr(0, bytesRead);
	}

	return req;
}

// returns empty pair if empty line is found
pair<string, string> parseHeader(const string &line)
{
	string headerLine = line;
	if (!headerLine.empty() && headerLine[headerLine.length() - 1] == '\r')
		headerLine.erase(headerLine.length() - 1);
	if (headerLine.empty())
	{
		return make_pair("", "");
	}

	vector<string> vec = utils::splitFirst(headerLine, ':');
	if (vec.size() != 2)
		throw runtime_error("BAD REQUEST: invalid header format");

	vec[0] = utils::toLower(vec[0]);
	if (!utils::isPrint(vec[0]) || vec[0].find(' ') != string::npos
		|| vec[0].find('\r') != string::npos
		|| vec[0].find('\n') != string::npos)
		throw runtime_error("BAD REQUEST: invalid header type");

	string value = utils::trim(vec[1], " ");
	if (!utils::isPrint(value) || value.find("\r\n") != string::npos)
		throw runtime_error("BAD REQUEST: invalid header content");

	return make_pair(vec[0], value);
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
