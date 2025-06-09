/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imsolucas <imsolucas@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 23:43:16 by imsolucas         #+#    #+#             */
/*   Updated: 2025/06/09 12:30:02 by imsolucas        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "colors.h"

Server::Server() : root("/var/www/html"),
				   client_max_body_size(1 * 1024 * 1024) // 1 MB default
{
}
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

void Server::addIndex(const std::string &index)
{
	if (std::find(indexes.begin(), indexes.end(), index) == indexes.end())
		indexes.push_back(index);
}

void Server::setRoot(const std::string &root) { this->root = root; }

void Server::setClientMaxBodySize(size_t size) { client_max_body_size = size; }

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

std::vector<int> Server::getPorts() const { return ports; }
std::vector<std::string> Server::getServerNames() const { return server_names; }
std::string Server::getRoot() const { return root; }
std::vector<Location> Server::getLocations() const { return locations; }
std::map<int, std::string> Server::getErrorPages() const { return error_pages; }

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

	std::cout << BOLD << MAGENTA << "Index Files: " << RESET;
	for (size_t i = 0; i < indexes.size(); ++i) {
		std::cout << GREEN << indexes[i];
		if (i + 1 < indexes.size()) std::cout << ", ";
	}
	std::cout << RESET << std::endl;

	std::cout << BOLD << MAGENTA << "Client Max Body Size: " << GREEN << client_max_body_size << " bytes" << RESET << std::endl;

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

