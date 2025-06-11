# pragma once

# include <poll.h>
# include <string>
# include <vector>

struct utils
{
	// pollUtils.cpp
	static void addToPoll(std::vector<pollfd> &poll, int fd, short events, short revents);
	static void removeFromPoll(std::vector<pollfd> &poll, int i);

	static int hexStrToInt(const std::string& hexStr);

	static void printError(std::string message);
	static void printError(const char* message);

	static std::vector<std::string> split(const std::string &str, char delimiter);

	// changeCase.cpp
	static std::string toLower(const std::string &string);
	static std::string toUpper(const std::string &string);

	template <typename T>
	static std::string toString(const T &any);
};

# include "../src/utils/string.tpp"
