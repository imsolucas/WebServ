# pragma once

# include <poll.h>
# include <string>
# include <vector>

struct utils
{
	// poll utils
	static void addToPoll(std::vector<pollfd> &poll, int fd, short events, short revents);
	static void removeFromPoll(std::vector<pollfd> &poll, int i);

	static int hexStrToInt(const std::string& hexStr);

	template <typename T>
	static std::string toString(const T &any);

	static std::vector<std::string> split(const std::string &str, char delimiter);

	static void printError(std::string message);
	static void printError(const char* message);
};

# include "../src/utils/string.tpp"
