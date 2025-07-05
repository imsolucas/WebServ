# pragma once

# include <map>
# include <vector>

class Server;

class ListenerManager
{
	public:
		void _setupAllListeners(const std::vector<Server>&servers);

		bool isListener(int fd);

		const std::map<int, int> &getListenerMap() const;
		int getPort(int listenerFd) const;

	private:
		// maps listener fd to listener port
		std::map<int, int> _listenerMap;

		void _setUpListener(int port);
		bool _isPortAlreadyListening(int port) const;
};
