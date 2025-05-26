# pragma once

#include "Includes.h"
#include "Server.hpp"
#include "LocationConfig.hpp"
# include <iostream>

class Config
{
	public:
		Config() {};
		Config(const std::string &filePath);
		~Config() {};

		void readConfig(const std::string &filePath);
		std::vector<Server> getServers() const;
		std::vector<std::string> tokenize(const std::string &str);
		void printline(const std::string &line) const;

	private:
		std::vector<Server> _servers; // Vector to hold multiple Server configurations
		void parseTokens(const std::vector<std::string> &tokens);
		Server parseServerBlock(std::vector<std::string>::const_iterator &it, std::vector<std::string>::const_iterator end);
		LocationConfig parseLocationBlock(std::vector<std::string>::const_iterator &it, std::vector<std::string>::const_iterator end);


};
