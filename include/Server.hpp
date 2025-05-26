/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imsolucas <imsolucas@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 15:43:47 by imsolucas         #+#    #+#             */
/*   Updated: 2025/05/27 00:58:29 by imsolucas        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

#include "LocationConfig.hpp"

class Server
{
public:
	// Constructors & Destructor
	Server();
	~Server();

	void setRoot(const std::string &root);
	void addErrorPage(int code, const std::string &path);
	void addLocation(const LocationConfig &location);
	void addPort(int port);
	void addServerName(const std::string &);
	void addIndex(const std::string &);
	void setClientMaxBodySize(size_t);
	void removeLocation(const std::string &path);
	void clearLocations();

	// Getters
	std::vector<int> getPorts() const;
	std::vector<std::string> getServerNames() const;
	std::string getRoot() const;
	std::vector<std::string> getIndexes() const;
	std::vector<LocationConfig> getLocations() const;
	std::map<int, std::string> getErrorPages() const;

	// Utils
	void printConfig() const;

private:
	std::vector<int> ports;				   // e.g., 8080, 8081
	std::vector<std::string> server_names; // e.g., "example.com"
	std::string root;					   // e.g., "/var/www/html"
	std::vector<std::string> indexes;
	size_t client_max_body_size;			// e.g., "index.html"
	std::vector<LocationConfig> locations;	// All `location {}` blocks
	std::map<int, std::string> error_pages; // Error code -> page path
};

#endif // SERVER_HPP
