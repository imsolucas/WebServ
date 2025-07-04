# include "Server.hpp"
# include "colors.h"

# include <algorithm> // find

using std::cerr;
using std::cout;
using std::endl;
using std::invalid_argument;
using std::map;
using std::pair;
using std::string;
using std::vector;

Server::Server()
: client_max_body_size(pair<size_t, string>(1, "MB")) {} // 1 MB default

Server::~Server() {}

void Server::addPort(int port)
{
	if (find(ports.begin(), ports.end(), port) == ports.end())
		ports.push_back(port);
}

void Server::addServerName(const string &server_name)
{
	if (find(server_names.begin(), server_names.end(), server_name) == server_names.end())
		server_names.push_back(server_name);
}

void Server::setClientMaxBodySize(size_t size, const string &unit)
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
		cerr << RED << "Invalid unit for client_max_body_size: " << unit << RESET << '\n';
		throw invalid_argument("Invalid unit for client_max_body_size");
	}
}

void Server::addErrorPage(int code, const string &path)
{
	error_pages[code] = path;
}

void Server::addLocation(const Location &location)
{
	locations.push_back(location);
}

void Server::removeLocation(const string &path)
{
	for (vector<Location>::iterator it = locations.begin(); it != locations.end(); ++it)
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

const vector<int> &Server::getPorts() const { return ports; }
const vector<string> &Server::getServerNames() const { return server_names; }
const vector<Location> &Server::getLocations() const { return locations; }
const map<int, string> &Server::getErrorPages() const { return error_pages; }

size_t Server::getClientMaxBodySizeInBytes() const
{
	size_t size = client_max_body_size.first;
	string unit = client_max_body_size.second;

	if (unit == "KB" || unit == "kb")
		return size * 1024;
	if (unit == "MB" || unit == "mb")
		return size * 1024 * 1024;
	if (unit == "GB" || unit == "gb")
		return size * 1024 * 1024 * 1024;

	return size; // Default to bytes if no valid unit is provided
}

void Server::printConfig() const {
	cout << BOLD << CYAN << "=== Server Configuration ===" << RESET << '\n';

	cout << BOLD << MAGENTA << "Ports: " << RESET;
	for (size_t i = 0; i < ports.size(); ++i) {
		cout << GREEN << ports[i];
		if (i + 1 < ports.size()) cout << ", ";
	}
	cout << RESET << '\n';

	cout << BOLD << MAGENTA << "Server Names: " << RESET;
	for (size_t i = 0; i < server_names.size(); ++i) {
		cout << GREEN << server_names[i];
		if (i + 1 < server_names.size()) cout << ", ";
	}
	cout << RESET << '\n';


	cout << BOLD << MAGENTA << "Client Max Body Size: " << GREEN
			  << client_max_body_size.first << " " << client_max_body_size.second
			  << RESET << endl;

	if (!error_pages.empty()) {
		cout << BOLD << MAGENTA << "Error Pages: " << RESET << '\n';
		for (map<int, string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
			cout << "  " << it->first << " => " << it->second << '\n';
		}
	}

	cout << endl;

	if (!locations.empty()) {
		cout << BOLD << CYAN << "--- Locations ---" << RESET << '\n';
		for (vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
			cout << *it;
		}
	}
	cout << BOLD << CYAN << "========================" << RESET << '\n';
	cout << '\n';
}
