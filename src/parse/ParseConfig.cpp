# include "WebServer.hpp"
# include "utils.hpp"

# include <algorithm> // count
# include <iostream>
# include <unistd.h> // close

using std::cerr;
using std::cout;
using std::count;
using std::endl;
using std::getline;
using std::ifstream;
using std::istringstream;
using std::ostringstream;
using std::runtime_error;
using std::string;
using std::stringstream;
using std::vector;

vector<Server> WebServer::_parseConfig(const string &filePath)
{
	string line;
	ifstream file(filePath.c_str());
	stringstream _buffer;
	size_t lineNumber = 0;
	bool hasMeaningfulLine = false;

	if (!file.is_open())
		throw runtime_error("Failed to open configuration file: " + filePath);

	while (getline(file, line))
	{
		lineNumber++;
		size_t commentPos = line.find('#');
		if (commentPos != string::npos)
			line = line.substr(0, commentPos); // Remove comments
		string trimmed = utils::trim(line, " \t\r\n");
		if (trimmed.empty())
			continue;
		hasMeaningfulLine = true; // Found a non-empty line
		if (!_checkLineSyntax(trimmed, lineNumber))
		{
			ostringstream oss;
			oss << "Invalid syntax at line " << lineNumber << ": " << trimmed;
			throw runtime_error(oss.str());
		}
		_buffer << line << "\n"; // Add line to buffer
	}
	file.close();
	if (!hasMeaningfulLine)
		throw runtime_error("Configuration file is logically empty: " + filePath);
	istringstream buffer(_buffer.str());

	vector<string> tokens = tokenize(buffer.str());
	vector<Server> servers = _parseTokens(tokens);
	return servers;
}

vector<string> WebServer::_getSemicolonDirectives() const {
	vector<string> directives;
	directives.push_back("listen");
	directives.push_back("server_name");
	directives.push_back("index");
	directives.push_back("root");
	directives.push_back("error_page");
	directives.push_back("client_max_body_size");
	directives.push_back("limit_except");
	directives.push_back("redirect");
	directives.push_back("return");
	return directives;
}

bool WebServer::_checkLineSyntax(const string &line, size_t lineNumber) const
{
	vector<string> mustEndWithSemicolon = _getSemicolonDirectives();

	for (size_t i = 0; i < mustEndWithSemicolon.size(); ++i)
	{
		const string &directive = mustEndWithSemicolon[i];
		if (line.find(directive) != string::npos &&
			line.find("{") == string::npos &&
			line.find("}") == string::npos &&
			!line.empty() && line[line.length() - 1] != ';')
		{
			cerr << "[Syntax Error] Line " << lineNumber
					  << ": missing ';' after '" << directive << "'\n'";
			return false;
		}
	}

	int openCount = count(line.begin(), line.end(), '{');
	int closeCount = count(line.begin(), line.end(), '}');
	if (openCount > 1 || closeCount > 1)
	{
		cerr << "[Syntax Error] Line" << lineNumber << ": too many braces on one line\n";
		return false;
	}
	return true;
}

const vector<Server> &WebServer::getServers() const
{
	return _servers;
}

void WebServer::printTokens(const vector<string> &tokens) const
{
	for (vector<string>::const_iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		cout << *it << endl;
	}
}

vector<string> WebServer::tokenize(const string &str)
{
	vector<string> tokens;
	string token;

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
			tokens.push_back(string(1, c));
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

vector<Server> WebServer::_parseTokens(const vector<string> &tokens)
{
	vector<Server> servers;
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
				throw runtime_error("Expected '{' after 'server'");
			++i; // Move past '{'
			Server server = parseServerBlock(tokens, i);
			server.printConfig();
			servers.push_back(server);
		}
		else
		{
			throw runtime_error("Unknown directive: " + tokens[i]);
		}
	}
	return servers;
}

Server WebServer::parseServerBlock(const vector<string> &tokens, size_t &i)
{
	Server server;

	while (i < tokens.size())
	{
		if (tokens[i] == ";")
		{
			++i; // Skip empty statement
			continue;
		}
		const string &token = tokens[i];
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
				server.addPort(atoi(tokens[i].c_str()));
				i++;
			}
			if (i >= tokens.size() || tokens[i] != ";")
				throw runtime_error("Missing ';' after listen");
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
				throw runtime_error("Missing ';' after server_name");
			++i;
		}
		else if (token == "error_page")
		{
			if (i + 2 >= tokens.size())
				throw runtime_error("Invalid error_page");
			int code = atoi(tokens[i + 1].c_str());
			string page = tokens[i + 2];
			server.addErrorPage(code, page);
			i += 3;
		}
		else if (token == "client_max_body_size")
		{
			if (i + 2 >= tokens.size())
				throw runtime_error("Invalid 'client_max_body_size' syntax");

			size_t size = static_cast<size_t>(atoi(tokens[i + 1].c_str()));
			string unit = "MB"; // Default to MB

			if (tokens[i + 2] != ";")
			{
				unit = tokens[i + 2];
				i++; // Account for the unit token
			}

			server.setClientMaxBodySize(size, unit);
			i += 2; // Skip size + optional unit
			if (tokens[i] != ";")
				throw runtime_error("Expected ';' after client_max_body_size");
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
			throw runtime_error("Unknown directive: " + token);
		}
	}
	return server;
}

Location WebServer::parseLocationBlock(const vector<string> &tokens, size_t &i)
{
	if (i >= tokens.size())
		throw runtime_error("Expected location path");

	string path = tokens[i++];
	Location loc(path);

	if (i >= tokens.size() || tokens[i] != "{")
		throw runtime_error("Expected '{' after location path");

	++i; // Move past '{'
	while (i < tokens.size())
	{
		if (tokens[i] == ";")
		{
			++i; // Skip empty statement
			continue;
		}
		const string &token = tokens[i];
		if (token == "}")
		{
			++i;
			break; // End of location block
		}
		else if (token == "root")
		{
			if (i + 2 >= tokens.size() || tokens[i + 2] != ";")
				throw runtime_error("Invalid 'root' syntax in location block");
			loc.setRoot(tokens[i + 1]);
			i += 3; // Move past "root <path>;"
		}
		else if (token == "index")
		{
			if (i + 2 >= tokens.size() || tokens[i + 2] != ";")
				throw runtime_error("Invalid 'index' syntax in location block");
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
				throw runtime_error("Invalid 'limit_except' syntax");
			i++; // Move past ";"
		}
		else if (token == "client_max_body_size")
		{
			if (i + 2 >= tokens.size())
				throw runtime_error("Invalid 'client_max_body_size' syntax");

			size_t size = static_cast<size_t>(atoi(tokens[i + 1].c_str()));
			string unit = "MB"; // Default to MB

			if (tokens[i + 2] != ";")
			{
				unit = tokens[i + 2];
				i++; // Account for the unit token
			}

			loc.setClientMaxBodySize(size, unit);
			i += 2; // Skip size + optional unit
			if (tokens[i] != ";")
				throw runtime_error("Expected ';' after client_max_body_size");
			i++; // Skip ';'
		}
		else if (token == "redirect")
		{
			if (i + 2 >= tokens.size() || tokens[i + 2] != ";")
				throw runtime_error("Invalid 'redirect' syntax in location block");
			loc.setRedirect(tokens[i + 1]);
			i += 3; // Move past "redirect <url>;"
		}
		else if (token == "return")
		{
			if (i + 2 >= tokens.size())
				throw runtime_error("Invalid return directive");
			string redirect = tokens[i + 1] + " " + tokens[i + 2];
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
			throw runtime_error("Unknown directive in location block: " + token);
		}
	}
	return loc;
}
