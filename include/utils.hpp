# pragma once

# include <poll.h>
# include <string>
# include <vector>

struct utils
{
	// poll utils
	static void addToPoll(std::vector<pollfd> &poll, int fd, short events, short revents);
	static void removeFromPoll(std::vector<pollfd> &poll, int i);

	template <typename T>
	static bool contains(const T &element, const std::vector<T> &vec);

	template <typename T>
	static std::string toString(const T &any);

	static std::vector<std::string> split(const std::string &str, char delimiter);
	static std::string toLowerStr(const std::string &str);

	static void printError(std::string message);
};

# include "../src/utils/string.tpp"
# include "../src/utils/utils.tpp"
