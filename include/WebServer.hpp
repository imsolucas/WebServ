# pragma once

# include <iostream>

# include "Config.hpp"

# include "Listener.hpp"

class WebServer
{
	public:
		WebServer(const std::string &config);

		void run();

	private:
		Config _cfg;
		Listener _listener;
		std::vector<pollfd> _fds;

		void _init();
		void _parse(const std::string &config);
		void _addToPoll(int fd, short events, short revents);


};
