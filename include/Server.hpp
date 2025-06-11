# pragma once

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include <algorithm>

# include "Location.hpp"

class Server
{
public:
	// Constructors & Destructor
	Server();
	~Server();

	void setRoot(const std::string &root);
	void addErrorPage(int code, const std::string &path);
	void addLocation(const Location &location);
	void addPort(int port);
	void addServerName(const std::string &);
	void addIndex(const std::string &);
	void setClientMaxBodySize(size_t size, const std::string &unit);
	void removeLocation(const std::string &path);
	void clearLocations();

	// Getters
	std::vector<int> getPorts() const;
	std::vector<std::string> getServerNames() const;
	std::string getRoot() const;
	std::vector<std::string> getIndexes() const;
	std::vector<Location> getLocations() const;
	std::map<int, std::string> getErrorPages() const;
	size_t getClientMaxBodySizeInBytes() const;

	// Utils
	void printConfig() const;

private:
	std::vector<int> ports;				   // e.g., 8080, 8081
	std::vector<std::string> server_names; // e.g., "example.com"
	std::string root;					   // e.g., "/var/www/html"
	std::vector<std::string> indexes;
	std::pair<size_t, std::string> client_max_body_size;			// e.g., "index.html"
	std::vector<Location> locations;	// All `location {}` blocks
	std::map<int, std::string> error_pages; // Error code -> page path
};
