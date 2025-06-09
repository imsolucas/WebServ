# pragma once

# include "Server.hpp"
# include "Location.hpp"

# include <sstream>
# include <stdexcept>
# include <fstream>


class Config
{
public:
	Config();
	Config(const std::string &filePath);
	~Config();

	void readConfig(const std::string &filePath);

	std::vector<Server> getServers() const;
	void printline(const std::string &line) const;
	void printTokens(const std::vector<std::string> &tokens) const;

	std::vector<std::string> tokenize(const std::string &str);

private:
	std::vector<Server> _servers; // Vector to hold multiple Server configurations
	void parseTokens(const std::vector<std::string> &tokens);
	Server parseServerBlock(const std::vector<std::string> &tokens, size_t &i);
	Location parseLocationBlock(const std::vector<std::string> &tokens, size_t &i);
};
