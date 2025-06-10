# include "utils.hpp"

#include <iostream>

using std::istringstream;
using std::runtime_error;

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
