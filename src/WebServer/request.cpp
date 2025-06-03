# include "Http.h"
# include "WebServer.hpp"

void WebServer::_handleRequest(const HttpRequest &request)
{
	// TODO: find correct virtual server based on host
	if (request.method == Http::GET)
		_handleGET();
	if (request.method == Http::POST)
		_handlePOST();
	if (request.method == Http::DELETE)
		_handleDELETE();

	// 2. loop through each location block and find closest matching prefix, otherwise use location /
	// 3. use `root` directive

	// 4. read and serialize the file requested in the request
}

void WebServer::_handleGET()
{

}

void WebServer::_handlePOST()
{

}

void WebServer::_handleDELETE()
{

}
