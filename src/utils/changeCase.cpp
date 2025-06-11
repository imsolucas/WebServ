# include "utils.hpp"

# include <algorithm> // transform()
# include <cctype> // tolower(), toupper()
# include <string>

using std::string;

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
