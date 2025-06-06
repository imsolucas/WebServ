#include "Config.hpp"

using std::cout;
using std::string;

Config::Config() : _servers() {}

Config::Config(const string &config)
{
	readConfig(config);
	cout << "Configuration file read successfully!\n";
}

Config::~Config() {}

void Config::readConfig(const string &filePath)
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
	parseTokens(tokens);
}

void Config::printTokens(const std::vector<std::string> &tokens) const
{
	for (std::vector<std::string>::const_iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
}

std::vector<Server> Config::getServers() const
{
	return _servers;
}

std::vector<std::string> Config::tokenize(const std::string &str)
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

void Config::parseTokens(const std::vector<std::string> &tokens)
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
}

Server Config::parseServerBlock(const std::vector<std::string> &tokens, size_t &i)
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
			LocationConfig loc = parseLocationBlock(tokens, i);
			server.addLocation(loc);
		}
		else
		{
			throw std::runtime_error("Unknown directive: " + token);
		}
	}
	return server;
}

LocationConfig Config::parseLocationBlock(const std::vector<std::string> &tokens, size_t &i)
{
	if (i >= tokens.size())
		throw std::runtime_error("Expected location path");
	
	std::string path = tokens[i++];
	LocationConfig loc(path);

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
