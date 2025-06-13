# include "utils.hpp"
# include "Http.h"
# include "WebServer.hpp"

using std::vector;
using std::string;
using std::cout;


void WebServer::_handleRequest(const HttpRequest &request)
{
	StatusCode code = OK;
	const Server &server = matchServer(request.headers.at("Host"), _servers);
	const Location &location = matchURI(request.requestTarget, server.getLocations());
	if (!utils::contains(request.method, location.getAllowedMethods()))
	{
		handleError(METHOD_NOT_ALLOWED);
		return;
	}
}

HttpResponse WebServer::_buildResponse()
{
	HttpResponse response;

	return response;
}

// TODO: remove this once merged Eu Ting's CGI PR
const Server &WebServer::matchServer(const string &host, const vector<Server> &servers)
{
	for (vector<Server>::const_iterator i = servers.begin();
		i != servers.end(); ++i)
	{
		const vector<string> &serverNames = (*i).getServerNames();
		for (vector<string>::const_iterator j = serverNames.begin();
			j != serverNames.end(); ++j)
		{
			if (host == *j) return *i;
		}
	}
	return servers[0];
}

// does longest prefix match
const Location &WebServer::matchURI(const string &URI, const vector<Location> &locations)
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

HttpResponse WebServer::handleError(StatusCode code)
{
	
}
