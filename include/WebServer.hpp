# pragma once

# include "ClientManager.hpp"
# include "Config.hpp"
# include "ListenerManager.hpp"

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
		ListenerManager _listenerManager;
		ClientManager _clientManager;

		std::vector<pollfd> _poll;
		size_t _pollIndex;

		void _handleListenerEvents(const pollfd &listener);
		void _handleClientEvents(const pollfd &client);

		static bool _noEvents(const pollfd &pollfd);
		static bool _clientIsConnecting(const pollfd &listener);
		static bool _clientIsDisconnected(const pollfd &client);
		static bool _clientIsSendingData(const pollfd &client);
		static bool _clientIsReadyToReceive(const pollfd &client);

	public:
		class PollException : public std::runtime_error
		{
			public:
				PollException();
		};
};
