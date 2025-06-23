# include "utils.hpp"

# include <algorithm> // transform()
# include <cctype> // tolower(), toupper()
# include <sstream>

using std::getline;
using std::istringstream;
using std::runtime_error;
using std::string;
using std::vector;

// util function to convert hexadecimal numbers to integer to get
// chunk size in CGI unchunking.
int utils::hexStrToInt(const std::string& hexStr)
{
	istringstream stream(hexStr);
	int value;

	stream >> std::hex >> value;

	// reject negative values because chunk size cannot be negative.
	if (stream.fail() || !stream.eof() || value < 0)
		throw runtime_error("Invalid hex string.");

	return value;
}

// std::transform expects a unary function that returns the same type as the input.
int toLowerChar(int c)
{
	return std::tolower(c);
}

string utils::toLower(const string &str)
{
	// lowercase must have allocated memory because we are passing lowercase.begin() to transform.
	string lowercase = str;

	std::transform(str.begin(), str.end(), lowercase.begin(), toLowerChar);

	return lowercase;
}

// std::transform expects a unary function that returns the same type as the input.
int toUpperChar(int c)
{
	return std::toupper(c);
}

string utils::toUpper(const string &str)
{
	// uppercase must have allocated memory because we are passing uppercase.begin() to transform.
	string uppercase = str;

	std::transform(str.begin(), str.end(), uppercase.begin(), toUpperChar);

	return uppercase;
}

// This helper function trims leading and trailing whitespaces from the input.
string utils::trim(const string& input, const string& whitespace)
{
	// Find the first non-whitespace character
	size_t start = input.find_first_not_of(whitespace);
	// npos (no-position) means no matches
	if (start == std::string::npos)
		return ""; // If only whitespace characters, return empty string.
	 // Find the last non-whitespace character
	size_t end = input.find_last_not_of(whitespace);
	return input.substr(start, end - start + 1);
}

vector<string> utils::split(const string &str, char delimiter)
{
	istringstream	iss(str);
	string			token;
	vector<string>	v;

	while (getline(iss, token, delimiter))
		v.push_back(token);

	return v;
}
