# include "utils.hpp"

#include <iostream>

using std::istringstream;
using std::runtime_error;

int utils::hexStrToInt(const std::string& hexStr)
{
	istringstream stream(hexStr);
	int value;

	stream >> std::hex >> value;

	if (stream.fail() || !stream.eof())
		throw runtime_error("Invalid hex string.");

	return value;
}
