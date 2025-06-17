# include <unistd.h>

# include "utils.hpp"
# include "CGIHandler.hpp"
# include "Http.h"
# include "ClientManager.hpp"

using std::vector;
using std::string;
using std::cout;

static bool isCGI(const string &file);

// TODO: handle directories
// TODO: autoindex
string ClientManager::_handleRequest(const ClientMeta &client)
{
	HttpResponse response;

	HttpRequest request = deserialize(client.requestBuffer);

	const Location &location = matchURI(request.requestTarget, client.server->getLocations());
		cout << "allowed methods 2: " + location.getAllowedMethodsAsString();
	if (!utils::contains(request.method, location.getAllowedMethods()))
	{
		cout << "method: " + request.method + "\n";
		response = handleError(METHOD_NOT_ALLOWED);
		return serialize(response);
	}

	string file = location.getRoot() + request.requestTarget;
	if (access(file.c_str(), F_OK) == -1)
	{
		response = handleError(NOT_FOUND);
		return serialize(response);
	}

	response = buildResponse(request, file);
	return serialize(response);
}

HttpResponse ClientManager::buildResponse(HttpRequest &request, const string &file)
{
	HttpResponse response;

	response.protocol = "HTTP/1.1";
	response.statusCode = OK;
	response.statusText = Http::statusText.find(OK)->second;

	if (isCGI(file))
	{
		if (access(file.c_str(), X_OK) == -1)
			return handleError(FORBIDDEN);
		CGIHandler cgi(request);
		StatusCode status = cgi.execute();
		if (status != OK)
			return handleError(status);
		response.body = cgi.getCGIOutput();
	}
	else if (request.method == Http::GET)
	{
		if (access(file.c_str(), R_OK) == -1)
			return handleError(FORBIDDEN);
		response.headers[Http::CONTENT_TYPE] = getContentType(file);
		response.body = utils::readFile(file);
	}
	response.headers[Http::SERVER] = "webserv";
	response.headers[Http::DATE] = utils::genTimeStamp();
	response.headers[Http::CONTENT_LENGTH] = utils::toString(response.body.size());

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
	cout << "allowed methods 1: " + bestMatch->getAllowedMethodsAsString() + "\n";
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
