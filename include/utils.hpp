# pragma once

# include <string>
# include <vector>

struct utils
{
	template <typename Input>
	static std::string toString(const Input &any);

	static std::vector<std::string> split(const std::string &str, char delimiter);
};

# include "../src/utils/string.tpp"
