# pragma once

# include <iostream>

# include "Config.hpp"

class WebServer
{
	public:
		WebServer(const std::string &config);

		void run();

	private:
		Config _cfg;

		void _init();
		void _parse(const std::string &config);
};
