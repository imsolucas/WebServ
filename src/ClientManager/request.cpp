# include <unistd.h>

# include "utils.hpp"
# include "Http.h"
# include "ClientManager.hpp"

using std::vector;
using std::string;

string ClientManager::_handleRequest(const ClientMeta &client)
{
	HttpResponse response;

	HttpRequest request = deserialize(client.requestBuffer);

	const Location &location = matchURI(request.requestTarget, client.server->getLocations());
	if (!utils::contains(request.method, location.getAllowedMethods()))
	{
		response = handleError(METHOD_NOT_ALLOWED);
		return serialize(response);
	}

	string path = location.getRoot() + request.requestTarget;
	PathType type = getPathType(path);
	switch (type)
	{
		case R_FILE:
			response = serveFile(request, path);
		break;

		case DIRECTORY:
			response = listDirectory(location, path);
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
