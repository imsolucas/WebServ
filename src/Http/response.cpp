# include <unistd.h>

# include "utils.hpp"
# include "CGIHandler.hpp"
# include "ClientManager.hpp"
# include "Http.h"

HttpResponse buildResponse(HttpRequest &request, const string &path)
{
	HttpResponse response;

	response.protocol = "HTTP/1.1";
	response.statusCode = OK;
	response.statusText = Http::statusText.find(OK)->second;

	if (isCGI(path))
	{
		if (access(path.c_str(), X_OK) == -1)
			return handleError(FORBIDDEN);
		CGIHandler cgi(request);
		StatusCode status = cgi.execute();
		if (status != OK)
			return handleError(status);
		response.body = cgi.getCGIOutput();
	}
	else if (request.method == Http::GET)
	{
		if (access(path.c_str(), R_OK) == -1)
			return handleError(FORBIDDEN);
		response.headers[Http::CONTENT_TYPE] = getContentType(path);
		response.body = utils::readFile(path);
	}
	response.headers[Http::SERVER] = "webserv";
	response.headers[Http::DATE] = utils::genTimeStamp();
	response.headers[Http::CONTENT_LENGTH] = utils::toString(response.body.size());

	return response;
}

// does longest prefix match
const Location &matchURI(const string &URI, const vector<Location> &locations)
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

HttpResponse handleError(StatusCode code)
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
