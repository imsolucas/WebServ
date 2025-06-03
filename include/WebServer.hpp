# pragma once

# include "Clients.hpp"
# include "Config.hpp"
# include "Listeners.hpp"

# include <map>
# include <poll.h>
# include <vector>

class WebServer
{
	public:
		WebServer(const std::string &config);
		~WebServer();

		void run();

	private:
		Config _cfg;
		Listeners _listeners;
		Clients _clients;

		std::vector<pollfd> _poll;
		size_t _pollIndex;

		void _handleListenerEvents(const pollfd &listener);
		void _handleClientEvents(const pollfd &client);

		static bool _noEvents(const pollfd &pollfd);

	public:
		class PollException : public std::runtime_error
		{
			public:
				PollException();
		};
};
