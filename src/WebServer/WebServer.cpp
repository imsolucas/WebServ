# include <iostream>

# include "WebServer.hpp"

using std::string;
using std::cout;

WebServer::WebServer(const string &config)
{
	_parse(config);
	_init();
}

void WebServer::run()
{
	_listener.setUpListener(8080);
	std::cout << "Listener set up !\n";
	// Event loop with poll()
	while (true)
	{
		// poll() returns the number of file descriptors that have had an event occur on them.

	}




	cout << "Web Server started !\n";
}

void WebServer::_init()
{
	cout << "Initialized Web Server !\n";
}

void WebServer::_parse(const string &config)
{
	_cfg = Config(config);
	cout << "Parsed configuration file !\n";

}
