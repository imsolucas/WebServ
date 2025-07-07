# include <cstdlib> // atoi
# include <unistd.h>

# include "colors.h"
# include "utils.hpp"
# include "Http.hpp"
# include "ClientManager.hpp"

using std::exception;
using std::vector;
using std::map;

string ClientManager::_handleRequest(const ClientMeta &client)
{
	const map<int, string> &errorPages = client.server->getErrorPages();
	HttpRequest request;
	HttpResponse response;

	if (client.requestMeta.maxBodySizeExceeded)
	{
		response = handleError(CONTENT_TOO_LARGE, errorPages);
		return serialize(response);
	}

	try
	{
		request = deserialize(client.requestBuffer);
	}
	catch (const exception &e)
	{
		utils::printError(e.what());
		if (string(e.what()).find("NOT IMPLEMENTED") != string::npos)
			response = handleError(NOT_IMPLEMENTED, errorPages);
		else
			response = handleError(BAD_REQUEST, errorPages);
		return serialize(response);
	}

	string requestTarget = request.requestTarget;
	if (requestTarget.find('?') != string::npos)
		requestTarget = utils::splitFirst(request.requestTarget, '?')[0];

	const Location *location = matchURI(requestTarget, client.server->getLocations());
	if (location == NULL)
	{
		response = handleError(NOT_FOUND, errorPages);
		return serialize(response);
	}
	if (!utils::contains(request.method, location->getAllowedMethods()))
	{
		response = handleError(METHOD_NOT_ALLOWED, errorPages);
		return serialize(response);
	}
	if (request.body.size() > location->getClientMaxBodySizeInBytes())
	{
		response = handleError(CONTENT_TOO_LARGE, errorPages);
		return serialize(response);
	}

	vector<string> redirection = utils::splitFirst(location->getRedirect(), ' ');
	if (!redirection[0].empty())
	{
		StatusCode code = (StatusCode)atoi(redirection[0].c_str());
		response = redirect(code, redirection[1]);
		return serialize(response);
	}

	string path;
	string root = utils::splitFirst(location->getRoot(), '/')[0];
	if (root.empty())
		path = location->getRoot() + requestTarget;
	else
		path = root + requestTarget;
	PathType type = getPathType(path);
	switch (type)
	{
		case R_FILE:
			response = serveFile(request, path, errorPages);
		break;

		case DIRECTORY:
			if (path[path.length() - 1] != '/')
				response = handleError(NOT_FOUND, errorPages);
			else
				response = listDirectory(*location, path, errorPages);
		break;

		case NOT_EXIST:
			response = handleError(NOT_FOUND, errorPages);
		break;

		default:
			response = handleError(FORBIDDEN, errorPages);
		break;
	}

	return serialize(response);
}
