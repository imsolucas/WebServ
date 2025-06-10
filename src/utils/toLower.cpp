# include "utils.hpp"

# include <algorithm> // transform()
# include <cctype> // tolower()
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
