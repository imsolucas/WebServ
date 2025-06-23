# pragma once

# include <string>
# include <vector>

namespace utils
{
	// file.cpp
	std::string readFile(const std::string &path);
	std::vector<std::string> readDirectory(const std::string &path);

	// printError.cpp
	void printError(std::string message);
	void printError(const char* message);

	// string.cpp
	bool isPrint(const std::string &string);
	bool isNum(const std::string &string);
	std::string toLower(const std::string &string);
	std::string toUpper(const std::string &string);
	std::string genTimeStamp();
	int hexStrToInt(const std::string& hexStr);
	std::string trim(const std::string& input, const std::string& whitespace);
	std::vector<std::string> splitFirst(const std::string &str, char demimiter);
	std::vector<std::string> split(const std::string &str, char delimiter);

	// string.tpp
	template <typename T>
	std::string toString(const T &any);

	template <typename T>
	bool contains(const T &element, const std::vector<T> &vec);
}

# include "../src/utils/string.tpp"
# include "../src/utils/utils.tpp"
