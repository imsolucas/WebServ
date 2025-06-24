// In ConfigChecker.cpp
#include "WebServer.hpp"
#include <iostream>
#include <set>

bool WebServer::_ConfigChecker(const std::vector<Server> &servers) const {
	bool valid = true;
	std::set<int> usedPorts;

	for (size_t s = 0; s < servers.size(); ++s) {
		const Server &server = servers[s];

		// Check if server has at least one listen port
		if (server.getPorts().empty()) {
			std::cerr << "[Error] Server block #" << s + 1 << " missing 'listen' directive.\n";
			valid = false;
		}

		// Check for duplicate listen ports
		for (std::vector<int>::const_iterator it = server.getPorts().begin(); it != server.getPorts().end(); ++it) {
			if (!usedPorts.insert(*it).second) {
				std::cerr << "[Warning] Duplicate 'listen' port found: " << *it << "\n";
			}
		}

		// Check server_name
		if (server.getServerNames().empty()) {
			std::cerr << "[Warning] Server block #" << s + 1 << " missing 'server_name'.\n";
		}

		// Check root directory
		if (server.getRoot().empty()) {
			std::cerr << "[Error] Server block #" << s + 1 << " missing 'root'.\n";
			valid = false;
		}

		// Validate locations
		const std::vector<Location> &locations = server.getLocations();
		for (size_t l = 0; l < locations.size(); ++l) {
			const Location &loc = locations[l];

			if (loc.getRoot().empty()) {
				std::cerr << "[Warning] Location '" << loc.getPath() << "' missing 'root'.\n";
			}

			if (loc.getAutoindex() && !loc.getIndex().empty()) {
				std::cerr << "[Warning] Location '" << loc.getPath() << "' has both 'index' and 'autoindex on'.\n";
			}

			if (loc.getAllowedMethods().empty()) {
				std::cerr << "[Warning] Location '" << loc.getPath() << "' missing 'limit_except', defaulting to allow all.\n";
			}
		}
	}

	return valid;
}
