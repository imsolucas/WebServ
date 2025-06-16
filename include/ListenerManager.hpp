# pragma once

# include <map>
# include <poll.h>
# include <vector>

class Server;

class ListenerManager
{
	public:
		ListenerManager(std::vector<pollfd> &_poll);

		void _setupAllListeners(const std::vector<Server>&servers);

		bool isListener(int fd);

		int getPort(int listenerFd) const;

	private:
		// maps listener fd to listener port
		std::map<int, int> _listenerMap;
		std::vector<pollfd> &_poll;

		void _setUpListener(int port);
};
