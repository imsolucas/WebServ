# include <unistd.h>

# include "colors.h"
# include "utils.hpp"
# include "Http.hpp"
# include "ClientManager.hpp"

using std::exception;
using std::vector;
using std::cerr;

// TODO: redirection
string ClientManager::_handleRequest(const ClientMeta &client)
{
	HttpResponse response;
	HttpRequest request;

	try
	{
		request = deserialize(client.requestBuffer);
	}
	catch (const exception &e)
	{
		cerr << RED << "Error: " << e.what() << "\n" << RESET;
		response = handleError(BAD_REQUEST);
		return serialize(response);
	}

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
