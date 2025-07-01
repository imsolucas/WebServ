#include "WebServer.hpp"

#include <iostream>
#include <unistd.h> // close
using std::runtime_error;

std::vector<Server> WebServer::_parseConfig(const std::string &filePath)
{
	std::string line;
	std::ifstream file(filePath.c_str());
	std::stringstream _buffer;
	size_t lineNumber = 0;
	bool hasMeaningfulLine = false;

	if (!file.is_open())
		throw std::runtime_error("Failed to open configuration file: " + filePath);

	while (std::getline(file, line))
	{
		lineNumber++;
		size_t commentPos = line.find('#');
		if (commentPos != std::string::npos)
			line = line.substr(0, commentPos); // Remove comments
		std::string trimmed = trim(line);
		if (trimmed.empty())
			continue;
		hasMeaningfulLine = true; // Found a non-empty line
		if (!_checkLineSyntax(trimmed, lineNumber))
		{
			std::ostringstream oss;
			oss << "Invalid syntax at line " << lineNumber << ": " << trimmed;
			throw std::runtime_error(oss.str());
		}
		_buffer << line << "\n"; // Add line to buffer
	}
	file.close();
	if (!hasMeaningfulLine)
		throw std::runtime_error("Configuration file is logically empty: " + filePath);
	std::istringstream buffer(_buffer.str());

	std::vector<std::string> tokens = tokenize(buffer.str());
	std::vector<Server> servers = _parseTokens(tokens);
	return servers;
}

std::vector<std::string> WebServer::_getSemicolonDirectives() const {
	std::vector<std::string> directives;
	directives.push_back("listen");
	directives.push_back("server_name");
	directives.push_back("index");
	directives.push_back("root");
	directives.push_back("error_page");
	directives.push_back("client_max_body_size");
	directives.push_back("limit_except");
	directives.push_back("cgi_path");
	directives.push_back("upload_store");
	directives.push_back("redirect");
	directives.push_back("return");
	return directives;
}

bool WebServer::_checkLineSyntax(const std::string &line, size_t lineNumber) const
{
	std::vector<std::string> mustEndWithSemicolon = _getSemicolonDirectives();

	for (size_t i = 0; i < mustEndWithSemicolon.size(); ++i)
	{
		const std::string &directive = mustEndWithSemicolon[i];
		if (line.find(directive) != std::string::npos &&
			line.find("{") == std::string::npos &&
			line.find("}") == std::string::npos &&
			!line.empty() && line[line.length() - 1] != ';')
		{
			std::cerr << "[Syntax Error] Line " << lineNumber
					  << ": missing ';' after '" << directive << "'\n'";
			return false;
		}
	}

	int openCount = std::count(line.begin(), line.end(), '{');
	int closeCount = std::count(line.begin(), line.end(), '}');
	if (openCount > 1 || closeCount > 1)
	{
		std::cerr << "[Syntax Error] Line" << lineNumber << ": too many braces on one line\n";
		return false;
	}
	return true;
}

std::string WebServer::trim(const std::string &str) const
{
	size_t start = str.find_first_not_of(" \t\r\n");
	size_t end = str.find_last_not_of(" \t\r\n");
	if (start == std::string::npos || end == std::string::npos)
		return "";
	return str.substr(start, end - start + 1);
}

const std::vector<Server> &WebServer::getServers() const
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
	std::vector<Server> servers;
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
			servers.push_back(server);
		}
		else
		{
			throw std::runtime_error("Unknown directive: " + tokens[i]);
		}
	}
	return servers;
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
			while (i < tokens.size() && tokens[i] != ";")
			{
				server.addServerName(tokens[i]);
				i++;
			}
			if (tokens[i] != ";")
				throw std::runtime_error("Missing ';' after server_name");
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

			size_t size = static_cast<size_t>(atoi(tokens[i + 1].c_str()));
			std::string unit = "MB"; // Default to MB

			if (tokens[i + 2] != ";")
			{
				unit = tokens[i + 2];
				i++; // Account for the unit token
			}

			server.setClientMaxBodySize(size, unit);
			i += 2; // Skip size + optional unit
			if (tokens[i] != ";")
				throw std::runtime_error("Expected ';' after client_max_body_size");
			i++; // Skip ';'
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
			loc.clearAllowedMethods(); // Clear previous methods
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
			if (i + 2 >= tokens.size())
				throw std::runtime_error("Invalid 'client_max_body_size' syntax");

			size_t size = static_cast<size_t>(atoi(tokens[i + 1].c_str()));
			std::string unit = "MB"; // Default to MB

			if (tokens[i + 2] != ";")
			{
				unit = tokens[i + 2];
				i++; // Account for the unit token
			}

			loc.setClientMaxBodySize(size, unit);
			i += 2; // Skip size + optional unit
			if (tokens[i] != ";")
				throw std::runtime_error("Expected ';' after client_max_body_size");
			i++; // Skip ';'
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
