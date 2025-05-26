/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imsolucas <imsolucas@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 15:43:47 by imsolucas         #+#    #+#             */
/*   Updated: 2025/05/26 15:43:47 by imsolucas        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "LocationConfig.hpp"

class Server {
public:
    // Constructors & Destructor
    Server();
    ~Server();

    // Setters
    void setPort(int port);
    void setServerName(const std::string &server_name);
    void setRoot(const std::string &root);
    void setIndex(const std::string &index);
    void addErrorPage(int code, const std::string &path);
    void addLocation(const LocationConfig &location);
    void removeLocation(const std::string &path);
    void clearLocations();

    // Getters
    int getPort() const;
    std::string getServerName() const;
    std::string getRoot() const;
    std::string getIndex() const;
    std::vector<LocationConfig> getLocations() const;
    std::map<int, std::string> getErrorPages() const;

    // Utils
    void printConfig() const;

private:
    int port;                                      // e.g., 8080
    std::string server_name;                       // e.g., "example.com"
    std::string root;                              // e.g., "/var/www/html"
    std::string index;                             // e.g., "index.html"
    std::vector<LocationConfig> locations;         // All `location {}` blocks
    std::map<int, std::string> error_pages;        // Error code -> page path
};

#endif // SERVER_HPP
