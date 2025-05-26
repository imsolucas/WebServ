
#include "Includes.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "Config.hpp"

#include <iostream>

using std::cout;
using std::string;

Config::Config(const string &config)
{
	readConfig(config);
	cout << "Configuration file read successfully!\n";
}

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
	// print the line with loops
	std::istringstream buffer(_buffer.str());
	while (std::getline(buffer, line))
	{
		printline(line);
	}

	// std::vector<std::string> tokens = tokenize(_buffer.str());
	// parseTokens(tokens);
}

void Config::printline(const std::string &line) const
{
	std::cout << line << std::endl;
}

std::vector<Server> Config::getServers() const
{
	return _servers;
}

// std::vector<std::string> Config::tokenize(const std::string &str)
// {
// 	return
// }
