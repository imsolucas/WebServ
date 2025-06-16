# include "utils.hpp"
# include "Http.h"
# include "ClientManager.hpp"

using std::vector;
using std::string;
using std::cout;


void ClientManager::_handleRequest(const ClientMeta &client)
{
	HttpRequest request = deserialize(client.requestBuffer);
	const Location &location = matchURI(request.requestTarget, client.server->getLocations());
	if (!utils::contains(request.method, location.getAllowedMethods()))
	{
		handleError(METHOD_NOT_ALLOWED);
		return;
	}
}

HttpResponse ClientManager::_buildResponse()
{
	HttpResponse response;

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
