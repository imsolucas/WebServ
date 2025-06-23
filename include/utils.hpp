# pragma once

# include <string>
# include <vector>

struct utils
{
	// printError.cpp
	static void printError(std::string message);
	static void printError(const char* message);

	// string.cpp
	static int hexStrToInt(const std::string& hexStr);
	static std::string toLower(const std::string &string);
	static std::string toUpper(const std::string &string);
	static std::string trim(const std::string& input, const std::string& whitespace);
	static std::vector<std::string> split(const std::string &str, char delimiter);

	// string.tpp
	template <typename T>
	static std::string toString(const T &any);
};

# include "../src/utils/string.tpp"
