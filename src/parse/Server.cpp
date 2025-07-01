#include "Server.hpp"
#include "colors.h"

Server::Server()
: root("/var/www/html"), client_max_body_size(std::pair<size_t, std::string>(1, "MB")) {} // 1 MB default

Server::~Server() {}

void Server::addPort(int port)
{
	if (std::find(ports.begin(), ports.end(), port) == ports.end())
		ports.push_back(port);
}

void Server::addServerName(const std::string &server_name)
{
	if (std::find(server_names.begin(), server_names.end(), server_name) == server_names.end())
		server_names.push_back(server_name);
}

// void Server::addIndex(const std::string &index)
// {
// 	if (std::find(indexes.begin(), indexes.end(), index) == indexes.end())
// 		indexes.push_back(index);
// }

void Server::setRoot(const std::string &root) { this->root = root; }

void Server::setClientMaxBodySize(size_t size, const std::string &unit)
{
	client_max_body_size.first = size;
	// client_max_body_size.second = unit;

	if (unit == "KB" || unit == "kb")
		client_max_body_size.second = "KB";
	else if (unit == "MB" || unit == "mb")
		client_max_body_size.second = "MB";
	else if (unit == "GB" || unit == "gb")
		client_max_body_size.second = "GB";
	else if (unit == "B" || unit == "b")
		client_max_body_size.second = "B";
	else
	{
		std::cerr << RED << "Invalid unit for client_max_body_size: " << unit << RESET << std::endl;
		throw std::invalid_argument("Invalid unit for client_max_body_size");
	}
}

void Server::addErrorPage(int code, const std::string &path)
{
	error_pages[code] = path;
}

void Server::addLocation(const Location &location)
{
	locations.push_back(location);
}

void Server::removeLocation(const std::string &path)
{
	for (std::vector<Location>::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		if (it->getPath() == path)
		{
			locations.erase(it);
			return;
		}
	}
}

void Server::clearLocations()
{
	locations.clear();
}

const std::vector<int> &Server::getPorts() const { return ports; }
const std::vector<std::string> &Server::getServerNames() const { return server_names; }
const std::string &Server::getRoot() const { return root; }
const std::vector<Location> &Server::getLocations() const { return locations; }
const std::map<int, std::string> &Server::getErrorPages() const { return error_pages; }

size_t Server::getClientMaxBodySizeInBytes() const
{
	size_t size = client_max_body_size.first;
	std::string unit = client_max_body_size.second;

	if (unit == "KB" || unit == "kb")
		return size * 1024;
	if (unit == "MB" || unit == "mb")
		return size * 1024 * 1024;
	if (unit == "GB" || unit == "gb")
		return size * 1024 * 1024 * 1024;

	return size; // Default to bytes if no valid unit is provided
}

void Server::printConfig() const {
	std::cout << BOLD << CYAN << "=== Server Configuration ===" << RESET << std::endl;

	std::cout << BOLD << MAGENTA << "Ports: " << RESET;
	for (size_t i = 0; i < ports.size(); ++i) {
		std::cout << GREEN << ports[i];
		if (i + 1 < ports.size()) std::cout << ", ";
	}
	std::cout << RESET << std::endl;

	std::cout << BOLD << MAGENTA << "Server Names: " << RESET;
	for (size_t i = 0; i < server_names.size(); ++i) {
		std::cout << GREEN << server_names[i];
		if (i + 1 < server_names.size()) std::cout << ", ";
	}
	std::cout << RESET << std::endl;

	std::cout << BOLD << MAGENTA << "Root: " << GREEN << root << RESET << std::endl;

	std::cout << BOLD << MAGENTA << "Client Max Body Size: " << GREEN
			  << client_max_body_size.first << " " << client_max_body_size.second
			  << RESET << std::endl;

	if (!error_pages.empty()) {
		std::cout << BOLD << MAGENTA << "Error Pages: " << RESET << std::endl;
		for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
			std::cout << "  " << it->first << " => " << it->second << std::endl;
		}
	}

	std::cout << std::endl;

	if (!locations.empty()) {
		std::cout << BOLD << CYAN << "--- Locations ---" << RESET << std::endl;
		for (std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
			it->printConfig();
		}
	}
	std::cout << BOLD << CYAN << "========================" << RESET << std::endl;
	std::cout << std::endl;
}

