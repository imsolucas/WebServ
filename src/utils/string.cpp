# include "utils.hpp"

# include <ctime>
# include <algorithm> // transform()
# include <cctype> // tolower(), toupper()
# include <sstream>

using std::getline;
using std::istringstream;
using std::runtime_error;
using std::string;
using std::vector;

string utils::genTimeStamp()
{
	time_t now = time(0);
	tm *gmt = gmtime(&now);

	char buffer[100];
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);

	return buffer;
}

// util function to convert hexadecimal numbers to integer to get
// chunk size in CGI unchunking.
int utils::hexStrToInt(const string& hexStr)
{
	istringstream iss(hexStr);
	int value;

	iss >> std::hex >> value;

	// reject negative values because chunk size cannot be negative.
	if (iss.fail() || !iss.eof() || value < 0)
		throw runtime_error("Invalid hex string.");

	return value;
}

bool utils::isPrint(const string &string)
{
	for (size_t i = 0; i < string.size(); ++i)
		if (!isprint(static_cast<unsigned char>(string[i])))
			return false;
	return true;
}

bool utils::isNum(const string &string)
{
	for (size_t i = 0; i < string.size(); ++i)
		if (!isdigit(string[i]))
			return false;
	return true;
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
	if (start == string::npos)
		return ""; // If only whitespace characters, return empty string.
	 // Find the last non-whitespace character
	size_t end = input.find_last_not_of(whitespace);
	return input.substr(start, end - start + 1);
}

// returns original string if substr is not found
string utils::erase(const string &str, const string &substr)
{
	size_t pos = str.find(substr);
	if (pos == string::npos) return str;

	string result = str;
	result.erase(pos, substr.length());
	return result;
}

// sets first element to empty string if delimiter is not found
vector<string> utils::splitFirst(const string &str, char delimiter) {
	vector<string> result;

	size_t pos = str.find(delimiter);
	if (pos == string::npos) result.push_back("");
	else
	{
		result.push_back(str.substr(0, pos));
		result.push_back(str.substr(pos + 1));
	}
	return result;
}

vector<string> utils::split(const string &str, char delimiter)
{
	istringstream	iss(str);
	string			token;
	vector<string>	v;

	while (getline(iss, token, delimiter))
		if (!token.empty())
			v.push_back(token);

	return v;
}
