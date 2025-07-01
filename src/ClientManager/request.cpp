# include <unistd.h>

# include "colors.h"
# include "utils.hpp"
# include "Http.hpp"
# include "ClientManager.hpp"

using std::exception;
using std::vector;
using std::cerr;

string ClientManager::_handleRequest(const ClientMeta &client)
{
	HttpResponse response;
	HttpRequest request;

	if (client.requestMeta.maxBodySizeExceeded)
	{
		response = handleError(CONTENT_TOO_LARGE);
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
			response = handleError(NOT_IMPLEMENTED);
		else
			response = handleError(BAD_REQUEST);
		return serialize(response);
	}

	const Location *location = matchURI(request.requestTarget, client.server->getLocations());
	if (location == NULL)
	{
		response = handleError(NOT_FOUND);
		return serialize(response);
	}
	if (!utils::contains(request.method, location->getAllowedMethods()))
	{
		response = handleError(METHOD_NOT_ALLOWED);
		return serialize(response);
	}
	if (request.body.size() > location->getClientMaxBodySizeInBytes())
	{
		response = handleError(CONTENT_TOO_LARGE);
		return serialize(response);
	}

	vector<string> redirection = utils::splitFirst(location->getRedirect(), ' ');
	if (!redirection[0].empty())
	{
		StatusCode code = (StatusCode)atoi(redirection[0].c_str());
		response = redirect(code, redirection[1]);
		return serialize(response);
	}

	string path = location->getRoot() + request.requestTarget;
	PathType type = getPathType(path);
	switch (type)
	{
		case R_FILE:
			response = serveFile(request, path);
		break;

		case DIRECTORY:
			if (path[path.length() - 1] != '/')
				response = handleError(NOT_FOUND);
			else
				response = listDirectory(*location, path);
		break;

		case NOT_EXIST:
			response = handleError(NOT_FOUND);
		break;

		default:
			response = handleError(FORBIDDEN);
		break;
	}

	return serialize(response);
}
