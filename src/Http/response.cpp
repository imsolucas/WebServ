# include <unistd.h>
# include <sstream>

# include "utils.hpp"
# include "CGIHandler.hpp"
# include "ClientManager.hpp"
# include "Http.hpp"

using std::map;
using std::ostringstream;

HttpResponse serveFile(HttpRequest &request, const string &file, const map<int, string> &errorPages)
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
			return handleError(FORBIDDEN, errorPages);
		CGIHandler cgi(request, utils::splitFirst(file, '/')[0]);
		StatusCode status = cgi.execute();
		if (status != OK)
			return handleError(status, errorPages);
		response.statusCode = (StatusCode)cgi.getCGIStatusCode();
		map<StatusCode, string>::const_iterator statusIt = Http::statusText.find(response.statusCode);
		if (statusIt != Http::statusText.end())
			response.statusText = statusIt->second;
		else
			response.statusText = "Unknown";
		map<string, string> headers = cgi.getCGIHeaders();
		for (map<string, string>::const_iterator it = headers.begin();
			it != headers.end(); ++it)
		{
			response.headers[it->first] = it->second;
		}
		response.body = cgi.getCGIBody();
	}
	else if (request.method == Http::GET)
	{
		if (access(file.c_str(), R_OK) == -1)
			return handleError(FORBIDDEN, errorPages);
		response.headers[Http::CONTENT_TYPE] = getContentType(file);
		response.body = utils::readFile(file);
	}
	response.headers[Http::CONTENT_LENGTH] = utils::toString(response.body.size());

	return response;
}

HttpResponse listDirectory(const Location &location, const string &directory, const map<int, string> &errorPages)
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
		{
			response.body = autoindex(directory);
			if (response.body.empty())
				return handleError(FORBIDDEN, errorPages);
		}
		else
			return handleError(FORBIDDEN, errorPages);
	}
	else
	{
		filePath = directory + "/" + filePath;
		if (getPathType(filePath) == R_FILE)
		{
			if (access(filePath.c_str(), R_OK) == -1)
				return handleError(FORBIDDEN, errorPages);
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

HttpResponse redirect(StatusCode code, const string &path)
{
	HttpResponse response;

	response.protocol = "HTTP/1.1";
	response.statusCode = code;
	response.statusText = Http::statusText.find(code)->second;
	response.headers[Http::SERVER] = "webserv";
	response.headers[Http::DATE] = utils::genTimeStamp();
	response.headers["Location"] = path;
	response.headers[Http::CONTENT_LENGTH] = "0";

	return response;
}

HttpResponse handleError(StatusCode code, const map<int, string> &errorPages)
{
	HttpResponse response;
	string errorPage;

	response.protocol = "HTTP/1.1";
	response.statusCode = code;
	response.statusText = Http::statusText.find(code)->second;
	response.headers[Http::SERVER] = "webserv";
	response.headers[Http::DATE] = utils::genTimeStamp();
	response.headers[Http::CONTENT_TYPE] = "text/html";

	if (errorPages.find(code) != errorPages.end())
		errorPage = errorPages.find(code)->second;
	else
		errorPage = "public/error/" + utils::toString(code) + ".html";
	response.body = utils::readFile(errorPage);

	response.headers[Http::CONTENT_LENGTH] = utils::toString(response.body.size());

	return response;
}

// returns empty string if readDirectory() fails
string autoindex(const string &directory)
{
	ostringstream body;
	body << "<!DOCTYPE html>\n"
		 << "<html>\n"
		 << "<head><title>Directory Listing</title></head>\n"
		 << "<body>\n"
		 << "<h1>Directory Listing for " << directory << "</h1>\n"
		 << "<ul>\n";

	string root = directory.substr(directory.find('/'));
	vector<string> dirents = utils::readDirectory(directory);
	if (dirents.empty()) return "";
	for (vector<string>::const_iterator it = dirents.begin();
		it != dirents.end(); ++it)
	{
		body << "<li><a href=\"" + root + *it + "\">" + *it + "</a></li>\n";
	}

	body << "</ul>\n"
		 << "</body>\n"
		 << "</html>";

	return body.str();
}

// does exact path match
// returns NULL if nothing matches
const Location *matchURI(const string &URI, const vector<Location> &locations)
{
	string prefix = URI.substr(0, URI.find_last_of('/')) + "/";

	for (vector<Location>::const_iterator i = locations.begin();
		i != locations.end(); ++i)
	{
		if (prefix == i->getPath())
			return &(*i);
	}

	return NULL;
}
