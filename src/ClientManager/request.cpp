# include <unistd.h>

# include "utils.hpp"
# include "Http.h"
# include "ClientManager.hpp"

using std::vector;
using std::string;
using std::cout;

static bool isCGI(const string &file);

void ClientManager::_handleRequest(const ClientMeta &client)
{
	HttpRequest request = deserialize(client.requestBuffer);
	HttpResponse response = buildResponse(request, client.server->getLocations());
}

HttpResponse ClientManager::buildResponse(const HttpRequest &request, const vector<Location> &locations)
{
	HttpResponse response;

	const Location &location = matchURI(request.requestTarget, locations);
	if (!utils::contains(request.method, location.getAllowedMethods()))
		return handleError(METHOD_NOT_ALLOWED);
	string file = location.getRoot() + request.requestTarget;
	if (access(file.c_str(), F_OK) == -1)
		return handleError(NOT_FOUND);
	if (isCGI(file))
		cout << "execute CGI\n";
	else if (request.method == Http::GET)
	{

	}

	return response;
}

// does longest prefix match
const Location &ClientManager::matchURI(const string &URI, const vector<Location> &locations)
{
	const Location *bestMatch = &locations[0];
	size_t matchLen = 0;
	for (vector<Location>::const_iterator i = locations.begin();
		i != locations.end(); ++i)
	{
		string prefix = (*i).getPath();
		if (prefix != "/" && *(prefix.end() - 1) != '/')
			prefix += "/";
		if (URI.find(prefix) == 0 && prefix.length() > matchLen)
		{
			bestMatch = &(*i);
			matchLen = prefix.length();
		}
	}
	return *bestMatch;
}

HttpResponse ClientManager::handleError(StatusCode code)
{
	HttpResponse response;

	response.protocol = "HTTP/1.1";
	response.statusCode = code;
	response.statusText = Http::statusText.find(code)->second;
	response.headers[Http::CONTENT_TYPE] = "text/html";

	string errorPage = "public/error/" + utils::toString(code) + ".html";
	response.body = utils::readFile(errorPage);

	response.headers[Http::CONTENT_LENGTH] = utils::toString(response.body.size());

	return response;
}

bool isCGI(const string &file)
{
	vector<string> extensions;
	extensions.push_back(".cgi");
	extensions.push_back(".py");
	extensions.push_back(".php");
	extensions.push_back(".go");
	extensions.push_back(".c");
	extensions.push_back(".sh");

	size_t dotPos = file.find_last_of('.');
	if (dotPos == string::npos) return false;

	string extension = file.substr(dotPos);
	for (vector<string>::const_iterator it = extensions.begin();
		it != extensions.end(); ++it)
		if (extension == *it) return true;

	return false;
}
