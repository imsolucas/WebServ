#include "Includes.h"
#include "Location.hpp"

// Constructors
Location::Location()
	: path("/"),
	  root("/var/www/html"),
	  index(""),
	  redirect(""),
	  upload_store(""),
	  autoindex(false),
	  client_max_body_size(std::pair<size_t, std::string>(1, "MB")) // 1 MB default
{
	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
}

Location::Location(const std::string &path)
	: path(path),
	  root("/var/www/html"),
	  index(""),
	  redirect(""),
	  upload_store(""),
	  autoindex(false),
	  client_max_body_size(std::pair<size_t, std::string>(1, "MB")) // 1 MB default
{
	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
}

Location::~Location() {}

// Setters
void Location::setPath(const std::string &path) { this->path = path; }
void Location::setRoot(const std::string &root) { this->root = root; }
void Location::setIndex(const std::string &index) { this->index = index; }
void Location::setRedirect(const std::string &redirect) { this->redirect = redirect; }
void Location::setUploadStore(const std::string &upload_store) { this->upload_store = upload_store; }
void Location::setAutoindex(bool autoindex) { this->autoindex = autoindex; }

void Location::setClientMaxBodySize(size_t size, const std::string &unit)
{
	client_max_body_size.first = size;
	client_max_body_size.second = unit;
}

void Location::addAllowedMethod(const std::string &method)
{
	if (std::find(allowed_methods.begin(), allowed_methods.end(), method) == allowed_methods.end())
		allowed_methods.push_back(method);
}

void Location::removeAllowedMethod(const std::string &method)
{
	std::vector<std::string>::iterator it = std::find(allowed_methods.begin(), allowed_methods.end(), method);
	if (it != allowed_methods.end())
		allowed_methods.erase(it);
}

void Location::clearAllowedMethods()
{
	allowed_methods.clear();
}

// Getters
const std::string &Location::getPath() const { return path; }
const std::string &Location::getRoot() const { return root; }
const std::string &Location::getIndex() const { return index; }
const std::string &Location::getRedirect() const { return redirect; }
const std::string &Location::getUploadStore() const { return upload_store; }
bool Location::getAutoindex() const { return autoindex; }

size_t Location::getClientMaxBodySizeInBytes() const
{
	size_t size = client_max_body_size.first;
	std::string unit = client_max_body_size.second;

	if (unit == "KB" || unit == "kb")
		return size * 1024;
	if (unit == "MB" || unit == "mb")
		return size * 1024 * 1024;
	if (unit == "GB" || unit == "gb")
		return size * 1024 * 1024 * 1024;

	return size; // Assume bytes if no valid unit
}

const std::vector<std::string> &Location::getAllowedMethods() const
{
	return allowed_methods;
}

std::string Location::getAllowedMethodsAsString() const
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

// Utils
bool Location::isMethodAllowed(const std::string &method) const
{
	for (std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
	{
		if (*it == method)
			return true;
	}
	return false;
}

void Location::printConfig() const
{
	std::cout << BOLD << CYAN << "Location Config:" << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Path: " << GREEN << path << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Root: " << GREEN << root << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Index: " << GREEN << index << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Redirect: " << GREEN << redirect << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Upload Store: " << GREEN << upload_store << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Autoindex: " << GREEN << (autoindex ? "true" : "false") << RESET << std::endl;
	std::cout << BOLD << MAGENTA << "Client Max Body Size: " << GREEN
			  << client_max_body_size.first << " " << client_max_body_size.second
			  << RESET << std::endl;

	std::cout << BOLD << MAGENTA << "Allowed Methods: " << RESET;
	for (std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
	{
		std::cout << GREEN << *it;
		if (it + 1 != allowed_methods.end())
			std::cout << ", ";
	}
	std::cout << RESET << std::endl;
}
