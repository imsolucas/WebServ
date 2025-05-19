/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imsolucas <imsolucas@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 16:31:05 by imsolucas         #+#    #+#             */
/*   Updated: 2025/05/20 01:14:50 by imsolucas        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Includes.h"

class LocationConfig;

class Server 
{
	public:
		Server();
		~Server();
		
		void addLocation(const LocationConfig &location);
		void removeLocation(const std::string &path);
		void clearLocations();
		void printConfig() const;

		void setPort(int port);
		void setServerName(const std::string &server_name);
		void setIndex(const std::string &index);
		void setRoot(const std::string &root);
		void addErrorPage(int code, const std::string &path);
		
		int getPort() const;
		std::string getServerName() const;
		std::string getIndex() const;
		std::string getRoot() const;
		std::vector<LocationConfig> getLocations() const;
		std::map<int, std::string> getErrorPages() const;
		
	private:
		int port; // e.g., 8080
		std::string server_name; // e.g., "example.com"
		std::string index; // e.g., "index.html"
		std::string root; // e.g., "/var/www/html"
		
		std::vector<LocationConfig> locations; // vector of LocationConfig objects
		std::map<int, std::string> error_pages; // map of error pages
		
};

#endif