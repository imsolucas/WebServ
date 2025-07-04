# pragma once

# include "Location.hpp"

# include <map>
# include <string>
# include <vector>

class Server
{
	public:
		// Constructors & Destructor
		Server();
		~Server();

		void addErrorPage(int code, const std::string &path);
		void addLocation(const Location &location);
		void addPort(int port);
		void addServerName(const std::string &);
		void setClientMaxBodySize(size_t size, const std::string &unit);
		void removeLocation(const std::string &path);
		void clearLocations();

		// Getters
		const std::vector<int> &getPorts() const;
		const std::vector<std::string> &getServerNames() const;
		const std::vector<Location> &getLocations() const;
		const std::map<int, std::string> &getErrorPages() const;
		size_t getClientMaxBodySizeInBytes() const;

		// Utils
		void printConfig() const;

	private:
		std::vector<int> ports;									// e.g., 8080, 8081
		std::vector<std::string> server_names;					// e.g., "example.com"
		std::pair<size_t, std::string> client_max_body_size;
		std::vector<Location> locations;						// All `location {}` blocks
		std::map<int, std::string> error_pages; 				// Error code -> page path
};
