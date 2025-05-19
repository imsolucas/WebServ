/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imsolucas <imsolucas@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 00:46:48 by imsolucas         #+#    #+#             */
/*   Updated: 2025/05/20 01:08:48 by imsolucas        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"

LocationConfig::LocationConfig() : path("/"), root("/var/www/html"), index("index.html"), cgi_path("/usr/bin/php-cgi"), redirect(""), upload_store(""), autoindex(false), client_max_body_size(1 * 1024 * 1024)
{
}

LocationConfig::LocationConfig(const std::string &path) : path(path), root("/var/www/html"), index("index.html"), cgi_path("/usr/bin/php-cgi"), redirect(""), upload_store(""), autoindex(false), client_max_body_size(1 * 1024 * 1024)
{
}

LocationConfig::~LocationConfig()
{
}

bool LocationConfig::isMethodAllowed(const std::string &method) const
{
	for (std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
	{
		if (*it == method)
			return true;
	}
	return false;
}

void LocationConfig::setPath(const std::string &path)
{
	this->path = path;
}

void LocationConfig::setRoot(const std::string &root)
{
	this->root = root;
}

void LocationConfig::setIndex(const std::string &index)
{
	this->index = index;
}

void LocationConfig::setCgiPath(const std::string &cgi_path)
{
	this->cgi_path = cgi_path;
}

void LocationConfig::setRedirect(const std::string &redirect)
{
	this->redirect = redirect;
}

void LocationConfig::setUploadStore(const std::string &upload_store)
{
	this->upload_store = upload_store;
}

void LocationConfig::setAutoindex(bool autoindex)
{
	this->autoindex = autoindex;
}

void LocationConfig::setClientMaxBodySize(size_t size)
{
	this->client_max_body_size = size;
}

void LocationConfig::addAllowedMethod(const std::string &method)
{
	if (std::find(allowed_methods.begin(), allowed_methods.end(), method) == allowed_methods.end())
		allowed_methods.push_back(method);
}

void LocationConfig::removeAllowedMethod(const std::string &method)
{
	std::vector<std::string>::iterator it = std::find(allowed_methods.begin(), allowed_methods.end(), method);
	if (it != allowed_methods.end())
		allowed_methods.erase(it);
}

void LocationConfig::clearAllowedMethods()
{
	allowed_methods.clear();
}

void LocationConfig::printConfig() const
{
	// Print the configuration for debugging nicely looked with colors and bold
	std::cout << BOLD << CYAN << "Location Config:" << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Path: " << GREEN << path << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Root: " << GREEN << root << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Index: " << GREEN << index << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "CGI Path: " << GREEN << cgi_path << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Redirect: " << GREEN << redirect << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Upload Store: " << GREEN << upload_store << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Autoindex: " << GREEN << (autoindex ? "true" : "false") << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Client Max Body Size: " << GREEN << client_max_body_size << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Allowed Methods: " << RESET;
	for (std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
	{
		std::cout << GREEN << *it;
		if (it + 1 != allowed_methods.end())
			std::cout << ", ";
	}
	std::cout << RESET << std::endl;
}

std::string LocationConfig::getPath() const
{
	return path;
}

std::string LocationConfig::getRoot() const
{
	return root;
}

std::string LocationConfig::getIndex() const
{
	return index;
}

std::string LocationConfig::getCgiPath() const
{
	return cgi_path;
}

std::string LocationConfig::getRedirect() const
{
	return redirect;
}

std::string LocationConfig::getUploadStore() const
{
	return upload_store;
}

bool LocationConfig::getAutoindex() const
{
	return autoindex;
}

size_t LocationConfig::getClientMaxBodySize() const
{
	return client_max_body_size;
}

std::vector<std::string> LocationConfig::getAllowedMethods() const
{
	return allowed_methods;
}

std::string LocationConfig::getAllowedMethodsAsString() const
{
	std::string result;
	for (std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
	{
		result += *it;
		if (it + 1 != allowed_methods.end())
			result += ", ";
	}
	return result;
}
