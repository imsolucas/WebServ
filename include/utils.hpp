# pragma once

# include <string>
# include <vector>

struct utils
{
	template <typename T>
	static std::string toString(const T &any);

	static std::vector<std::string> split(const std::string &str, char delimiter);
};

# include "../src/utils/string.tpp"
