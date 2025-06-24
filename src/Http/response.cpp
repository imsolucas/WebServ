# include <unistd.h>
# include <sstream>

# include "utils.hpp"
# include "CGIHandler.hpp"
# include "ClientManager.hpp"
# include "Http.h"

using std::ostringstream;

HttpResponse serveFile(HttpRequest &request, const string &file)
{
	HttpResponse response;

	response.protocol = "HTTP/1.1";
	response.statusCode = OK;
	response.statusText = Http::statusText.find(OK)->second;
	response.headers[Http::SERVER] = "webserv";
	response.headers[Http::DATE] = utils::genTimeStamp();

	if (isCGI(file))
	{
		if (access(file.c_str(), X_OK) == -1)
			return handleError(FORBIDDEN);
		CGIHandler cgi(request);
		StatusCode status = cgi.execute();
		if (status != OK)
			return handleError(status);
		response.headers[Http::CONTENT_TYPE] = cgi.getCGIOutputType();
		response.body = cgi.getCGIOutput();
	}
	else if (request.method == Http::GET)
	{
		if (access(file.c_str(), R_OK) == -1)
			return handleError(FORBIDDEN);
		response.headers[Http::CONTENT_TYPE] = getContentType(file);
		response.body = utils::readFile(file);
	}
	response.headers[Http::CONTENT_LENGTH] = utils::toString(response.body.size());

	return response;
}

HttpResponse listDirectory(const Location &location, const string &directory)
{
	HttpResponse response;
	response.protocol = "HTTP/1.1";
	response.statusCode = OK;
	response.statusText = Http::statusText.find(OK)->second;
	response.headers[Http::SERVER] = "webserv";
	response.headers[Http::DATE] = utils::genTimeStamp();
	response.headers[Http::CONTENT_TYPE] = "text/html";

	string filePath = location.getIndex();
	if (filePath.empty())
	{
		if (location.getAutoindex() == true)
			response.body = autoindex(directory);
		else
			return handleError(FORBIDDEN);
	}
	else
	{
		filePath = directory + "/" + filePath;
		if (getPathType(filePath) == R_FILE)
		{
			if (access(filePath.c_str(), R_OK) == -1)
				return handleError(FORBIDDEN);
			response.body = utils::readFile(filePath);
		}
		else
		{
			if (location.getAutoindex() == true)
				response.body = autoindex(directory);
		}
	}
	response.headers[Http::CONTENT_LENGTH] = utils::toString(response.body.size());

	return response;
}

string autoindex(const string &directory)
{
	ostringstream body;
	body << "<!DOCTYPE html>\n"
		 << "<html>\n"
		 << "<head><title>Directory Listing</title></head>\n"
		 << "<body>\n"
		 << "<h1>Directory Listing for " << directory << "</h1>\n"
		 << "<ul>\n";

	vector<string> dirents = utils::readDirectory(directory);
	for (vector<string>::const_iterator it = dirents.begin();
		it != dirents.end(); ++it)
	{
		body << "<li><a href=\"" << *it << "\">" << *it << "</a></li>\n";
	}

	body << "</ul>\n"
		 << "</body>\n"
		 << "</html>";

	return body.str();
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
