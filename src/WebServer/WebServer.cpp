# include "WebServer.hpp"
# include "colors.h"
# include "signal.hpp"
# include "utils.hpp"

# include <iostream>
# include <unistd.h> // close

using std::cerr;
using std::cout;
using std::string;
using std::runtime_error;

WebServer::WebServer(const string &config) : _listenerManager(_poll), _clientManager(_poll, _pollIndex)
{
	_servers = _parseConfig(config);
	cout << "Parsed configuration file!\n";
	_listenerManager._setupAllListeners(_servers);
}

WebServer::~WebServer()
{
	for (size_t i = 0; i < _poll.size(); ++i)
		close(_poll[i].fd);
	cout << "Closed all sockets!\n";
}

// Event loop with poll()
void WebServer::run()
{
	cout << "Web server started!\n";
	while (!gStopLoop)
	{
		// -1 passed to poll() will cause the server to wait indefinitely for an event.
		// .data() returns a pointer to the first element in the array used internally by the vector.
		int ready = poll(_poll.data(), _poll.size(), -1);
		// poll() returns the number of file descriptors that have had an event occur on them.
		// return value: 0 = timeout (irrelavant with -1 timeout parameter); -1 = error
		// prevent double printing if SIGINT received
		if (ready < 0 && !gStopLoop)
			throw PollException();

		// for loop to service all file descriptors with events
		for (_pollIndex = 0; _pollIndex < _poll.size(); _pollIndex++)
		{
			pollfd &pfd = _poll[_pollIndex];

			if (_noEvents(pfd))
				continue;
			if (_listenerManager.isListener(pfd.fd))
				_handleListenerEvents(pfd);
			if (_clientManager.isClient(pfd.fd))
				_handleClientEvents(pfd);
		}
	}
}

void WebServer::_handleListenerEvents(const pollfd &listener)
{
	if (_clientIsConnecting(listener))
		_clientManager.addClient(listener.fd, _listenerManager.getPort(listener.fd));
}

void WebServer::_handleClientEvents(const pollfd &client)
{
	int fd = client.fd;

	if (_clientIsDisconnected(client))
		_clientManager.removeClient(fd);
	else if (_clientIsSendingData(client))
		_clientManager.recvFromClient(fd);
	else if (_clientIsReadyToReceive(client))
		_clientManager.sendToClient(fd);
}

bool WebServer::_noEvents(const pollfd &pfd)
{
	return pfd.revents == 0;
}

// POLLIN on listener means client is attempting to connect to the server
bool WebServer::_clientIsConnecting(const pollfd &listener)
{
	return listener.revents & POLLIN;
}

// problematic/disconnected clients
// POLLERR = socket error (I/O error or connection reset or unusable socket)
// POLLHUP = hang up (client disconnected)
// POLLNVAL = invalid fd (fd closed but still in _poll list)
bool WebServer::_clientIsDisconnected(const pollfd &client)
{
	return client.revents & (POLLERR | POLLHUP | POLLNVAL);
}

// POLLIN on client means client is sending data to the server
bool WebServer::_clientIsSendingData(const pollfd &client)
{
	return client.revents & POLLIN;
}

// POLLOUT on client means client is ready to receive data
bool WebServer::_clientIsReadyToReceive(const pollfd &client)
{
	return client.revents & POLLOUT;
}

WebServer::PollException::PollException()
: runtime_error("Failed to poll file descriptors.") {}

std::vector<Server> WebServer::_parseConfig(const std::string &filePath)
{
	std::string line;
	std::ifstream file(filePath.c_str());
	std::stringstream _buffer;

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open configuration file: " + filePath);
	}
	while (std::getline(file, line))
	{
		size_t commentPos = line.find('#');
		if (commentPos != std::string::npos)
		{
			line = line.substr(0, commentPos); // Remove comments
		}
		_buffer << line << "\n"; // Add line to buffer
	}
	file.close();
	std::istringstream buffer(_buffer.str());
	std::vector<std::string> tokens = tokenize(buffer.str());
	// printTokens(tokens);
	return _parseTokens(tokens);
}

std::vector<Server> WebServer::getServers() const
{
	return _servers;
}

void WebServer::printTokens(const std::vector<std::string> &tokens) const
{
	for (std::vector<std::string>::const_iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
}

std::vector<std::string> WebServer::tokenize(const std::string &str)
{
	std::vector<std::string> tokens;
	std::string token;

	for (size_t i = 0; i < str.size(); ++i)
	{
		char c = str[i];

		if (c == '{' || c == '}' || c == ';')
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
			tokens.push_back(std::string(1, c));
		}
		else if (isspace(c))
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
		}
		else
		{
			token += c;
		}
	}
	if (!token.empty())
	{
		tokens.push_back(token);
	}
	return tokens;
}

std::vector<Server> WebServer::_parseTokens(const std::vector<std::string> &tokens)
{
	size_t i = 0;

	while (i < tokens.size())
	{
		if (tokens[i] == ";")
		{
			++i; // Skip empty statement
			continue;
		}
		if (tokens[i] == "server")
		{
			++i; // Move past "server"
			if (i >= tokens.size() || tokens[i] != "{")
				throw std::runtime_error("Expected '{' after 'server'");
			++i; // Move past '{'
			Server server = parseServerBlock(tokens, i);
			server.printConfig();
			_servers.push_back(server);
		}
		else
		{
			throw std::runtime_error("Unknown directive: " + tokens[i]);
		}
	}
	return _servers;
}

Server WebServer::parseServerBlock(const std::vector<std::string> &tokens, size_t &i)
{
	Server server;

	while (i < tokens.size())
	{
		if (tokens[i] == ";")
		{
			++i; // Skip empty statement
			continue;
		}
		const std::string &token = tokens[i];
		if (token == "}")
		{
			++i;
			break; // End of server block
		}
		else if (token == "listen")
		{
			++i; // Move past "listen"
			while (i < tokens.size() && tokens[i] != ";")
			{
				server.addPort(std::atoi(tokens[i].c_str()));
				i++;
			}
			if (i >= tokens.size() || tokens[i] != ";")
				throw std::runtime_error("Missing ';' after listen");
			++i;
		}
		else if (token == "server_name")
		{
			i++;
			while (i < tokens.size() && tokens[i] != ";") {
				server.addServerName(tokens[i]);
				i++;
			}
			if (tokens[i] != ";")
				throw std::runtime_error("Missing ';' after server_name");
			++i;
		}
		else if (token == "index")
		{
			i++;
			while (i < tokens.size() && tokens[i] != ";") {
				server.addIndex(tokens[i]);
				i++;
			}
			if (tokens[i] != ";")
				throw std::runtime_error("Missing ';' after index");
			++i;
		}
		else if (token == "root")
		{
			if (i + 2 >= tokens.size() || tokens[i + 2] != ";")
				throw std::runtime_error("Invalid 'root' syntax");
			server.setRoot(tokens[i + 1]);
			i += 3; // Move past "root <path>;"
		}
		else if (token == "error_page")
		{
			if (i + 2 >= tokens.size())
				throw std::runtime_error("Invalid error_page");
			int code = std::atoi(tokens[i + 1].c_str());
			std::string page = tokens[i + 2];
			server.addErrorPage(code, page);
			i += 3;
		}
		else if (token == "client_max_body_size")
		{
			if (i + 2 >= tokens.size())
				throw std::runtime_error("Invalid 'client_max_body_size' syntax");
			server.setClientMaxBodySize(static_cast<size_t>(atoi(tokens[i + 1].c_str())));
			i += 3; // Move past "client_max_body_size <size>;"
		}
		else if (token == "location")
		{
			i++; // Move past "location"
			Location loc = parseLocationBlock(tokens, i);
			server.addLocation(loc);
		}
		else
		{
			throw std::runtime_error("Unknown directive: " + token);
		}
	}
	return server;
}

Location WebServer::parseLocationBlock(const std::vector<std::string> &tokens, size_t &i)
{
	if (i >= tokens.size())
		throw std::runtime_error("Expected location path");
	
	std::string path = tokens[i++];
	Location loc(path);

	if (i >= tokens.size() || tokens[i] != "{")
		throw std::runtime_error("Expected '{' after location path");
	
	++i; // Move past '{'
	while (i < tokens.size())
	{
		if (tokens[i] == ";")
		{
			++i; // Skip empty statement
			continue;
		}
		const std::string &token = tokens[i];
		if (token == "}")
		{
			++i;
			break; // End of location block
		}
		else if (token == "root")
		{
			if (i + 2 >= tokens.size() || tokens[i + 2] != ";")
				throw std::runtime_error("Invalid 'root' syntax in location block");
			loc.setRoot(tokens[i + 1]);
			i += 3; // Move past "root <path>;"
		}
		else if (token == "index")
		{
			if (i + 2 >= tokens.size() || tokens[i + 2] != ";")
				throw std::runtime_error("Invalid 'index' syntax in location block");
			loc.setIndex(tokens[i + 1]);
			i += 3; // Move past "index <file>;"
		}
		else if (token == "limit_except")
		{
			i++;
			while (i < tokens.size() && tokens[i] != ";")
			{
				loc.addAllowedMethod(tokens[i]);
				i++;
			}
			if (i >= tokens.size() || tokens[i] != ";")
				throw std::runtime_error("Invalid 'limit_except' syntax");
			i++; // Move past ";"
		}
		else if (token == "client_max_body_size")
		{
			if (i + 2 >= tokens.size() || tokens[i + 2] != ";")
				throw std::runtime_error("Invalid 'client_max_body_size' syntax in location block");
			loc.setClientMaxBodySize(static_cast<size_t>(atoi(tokens[i + 1].c_str())));
			i += 3; // Move past "client_max_body_size <size>;"
		}
		else if (token == "cgi_path")
		{
			if (i + 2 >= tokens.size() || tokens[i + 2] != ";")
				throw std::runtime_error("Invalid 'cgi_path' syntax in location block");
			loc.setCgiPath(tokens[i + 1]);
			i += 3; // Move past "cgi_path <path>;"
		}
		else if (token == "redirect")
		{
			if (i + 2 >= tokens.size() || tokens[i + 2] != ";")
				throw std::runtime_error("Invalid 'redirect' syntax in location block");
			loc.setRedirect(tokens[i + 1]);
			i += 3; // Move past "redirect <url>;"
		}
		else if (token == "upload_store")
		{
			if (i + 2 >= tokens.size() || tokens[i + 2] != ";")
				throw std::runtime_error("Invalid 'upload_store' syntax in location block");
			loc.setUploadStore(tokens[i + 1]);
			i += 3; // Move past "upload_store <path>;"
		}
		else if (token == "return")
		{
			if (i + 2 >= tokens.size())
				throw std::runtime_error("Invalid return directive");
			std::string redirect = tokens[i + 1] + " " + tokens[i + 2];
			loc.setRedirect(redirect);
			i += 3;
		}
		else if (token == "autoindex")
		{
			if (tokens[i + 1] == "on")
				loc.setAutoindex(true);
			else
				loc.setAutoindex(false);
			i += 3;
		}
		else
		{
			throw std::runtime_error("Unknown directive in location block: " + token);
		}
	}
	return loc;
}