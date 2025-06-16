# pragma once

# include <poll.h>
# include <string>
# include <vector>

struct utils
{
	// file.cpp
	static std::string readFile(const std::string &path);

	// pollUtils.cpp
	static void addToPoll(std::vector<pollfd> &poll, int fd, short events, short revents);
	static void removeFromPoll(std::vector<pollfd> &poll, int i);

	// printError.cpp
	static void printError(std::string message);
	static void printError(const char* message);

	// string.cpp
	static int hexStrToInt(const std::string& hexStr);
	static std::string toLower(const std::string &string);
	static std::string toUpper(const std::string &string);
	static std::vector<std::string> split(const std::string &str, char delimiter);

	// string.tpp
	template <typename T>
	static std::string toString(const T &any);

	template <typename T>
	static bool contains(const T &element, const std::vector<T> &vec);
};

# include "../src/utils/string.tpp"
# include "../src/utils/utils.tpp"
