# include "Config.hpp"

# include <iostream>

using std::string;
using std::cout;

Config::Config(const string &config)
{
	(void)config;
	cout << "Parsing config ...\n";
}
