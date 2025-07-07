# include "Location.hpp"
# include "colors.h"

# include <algorithm> // find

using std::ostream;
using std::pair;
using std::string;
using std::vector;

// Constructors
Location::Location()
	: path("/"),
	  root(""),
	  index(""),
	  redirect(""),
	  autoindex(false),
	  client_max_body_size(pair<size_t, string>(1, "MB")) // 1 MB default
{
	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
}

Location::Location(const string &path)
	: path(path),
	  root(""),
	  index(""),
	  redirect(""),
	  autoindex(false),
	  client_max_body_size(pair<size_t, string>(1, "MB")) // 1 MB default
{
	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
}

Location::~Location() {}

// Setters
void Location::setPath(const string &path) { this->path = path; }
void Location::setRoot(const string &root) { this->root = root; }
void Location::setIndex(const string &index) { this->index = index; }
void Location::setRedirect(const string &redirect) { this->redirect = redirect; }
void Location::setAutoindex(bool autoindex) { this->autoindex = autoindex; }

void Location::setClientMaxBodySize(size_t size, const string &unit)
{
	client_max_body_size.first = size;
	client_max_body_size.second = unit;
}

void Location::addAllowedMethod(const string &method)
{
	if (find(allowed_methods.begin(), allowed_methods.end(), method) == allowed_methods.end())
		allowed_methods.push_back(method);
}

void Location::removeAllowedMethod(const string &method)
{
	vector<string>::iterator it = find(allowed_methods.begin(), allowed_methods.end(), method);
	if (it != allowed_methods.end())
		allowed_methods.erase(it);
}

void Location::clearAllowedMethods()
{
	allowed_methods.clear();
}

// Getters
const string &Location::getPath() const { return path; }
const string &Location::getRoot() const { return root; }
const string &Location::getIndex() const { return index; }
const string &Location::getRedirect() const { return redirect; }
bool Location::getAutoindex() const { return autoindex; }

size_t Location::getClientMaxBodySizeInBytes() const
{
	size_t size = client_max_body_size.first;
	string unit = client_max_body_size.second;

	if (unit == "KB" || unit == "kb")
		return size * 1024;
	if (unit == "MB" || unit == "mb")
		return size * 1024 * 1024;
	if (unit == "GB" || unit == "gb")
		return size * 1024 * 1024 * 1024;

	return size; // Assume bytes if no valid unit
}

const vector<string> &Location::getAllowedMethods() const
{
	return allowed_methods;
}

string Location::getAllowedMethodsAsString() const
{
	string result;
	for (vector<string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
	{
		result += *it;
		if (it + 1 != allowed_methods.end())
			result += ", ";
	}
	return result;
}

// Utils
bool Location::isMethodAllowed(const string &method) const
{
	for (vector<string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
	{
		if (*it == method)
			return true;
	}
	return false;
}

bool Location::operator == (const Location &rhs) const
{
	return path == rhs.path &&
			root == rhs.root &&
			index == rhs.index &&
			redirect == rhs.redirect &&
			autoindex == rhs.autoindex &&
			client_max_body_size == rhs.client_max_body_size &&
			allowed_methods == rhs.allowed_methods;
}

ostream &operator << (ostream &os, const Location &rhs)
{
	os << BOLD << CYAN << "Location Config:" << RESET << '\n';
	os << BOLD << MAGENTA << "Path: " << GREEN << rhs.getPath() << RESET << '\n';
	os << BOLD << MAGENTA << "Root: " << GREEN << rhs.getRoot() << RESET << '\n';
	os << BOLD << MAGENTA << "Index: " << GREEN << rhs.getIndex() << RESET << '\n';
	os << BOLD << MAGENTA << "Redirect: " << GREEN << rhs.getRedirect() << RESET << '\n';
	os << BOLD << MAGENTA << "Autoindex: " << GREEN << (rhs.getAutoindex() ? "true" : "false") << RESET << '\n';
	os << BOLD << MAGENTA << "Client Max Body Size: " << GREEN
		<< rhs.getClientMaxBodySizeInBytes() << RESET << '\n';

	os << BOLD << MAGENTA << "Allowed Methods: " << RESET
		<< rhs.getAllowedMethodsAsString() << RESET << '\n';

	return os;
}
