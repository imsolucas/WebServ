# include <unistd.h>

# include "utils.hpp"
# include "Http.h"
# include "ClientManager.hpp"

using std::vector;
using std::string;
using std::cout;

// TODO: handle directories
// TODO: autoindex
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

	string path = location.getRoot() + request.requestTarget; // TODO
	cout << "path: " + path + "\n";
	if (access(path.c_str(), F_OK) == -1)
	{
		response = handleError(NOT_FOUND);
		return serialize(response);
	}

	response = buildResponse(request, path);
	return serialize(response);
}
