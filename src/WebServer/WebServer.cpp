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
