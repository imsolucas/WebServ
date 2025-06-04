# pragma once

# include <map>
# include <poll.h>
# include <stdexcept>
# include <vector>

class Server;

class ListenerManager
{
	public:
		ListenerManager(std::vector<pollfd> &_poll);

		void _setupAllListeners(const std::vector<Server>&servers);

		bool isListener(int fd);
		static bool clientIsConnecting(const pollfd &listener);

		int getPort(int listenerFd) const;

	private:
		// maps listener fd to listener port
		std::map<int, int> _listenerMap;
		std::vector<pollfd> &_poll;

		void _setUpListener(int port);

	public:
		// inherit from runtime_error to customize error message
		class SocketCreationException : public std::runtime_error
		{
			public:
				SocketCreationException(const std::string &portString);
		};

		class SocketConfigException : public std::runtime_error
		{
			public:
				SocketConfigException(const std::string &portString);
		};

		class SocketOptionException : public std::runtime_error
		{
			public:
				SocketOptionException(const std::string &portString);
		};

		class BindException : public std::runtime_error
		{
			public:
				BindException(const std::string &portString);
		};

		class ListenException : public std::runtime_error
		{
			public:
				ListenException(const std::string &portString);
		};
};
