# pragma once

# include "Config.hpp"

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

		std::vector<pollfd> _poll;
		size_t _pollIndex;

		// maps listener fd to listener port
		std::map<int, int> _listenerMap;

		struct ClientMeta
		{
			int port;
			int listenerFd;
			std::string requestBuffer;
			bool headersParsed;
			bool chunkedRequest;
			int contentLength;
		};

		// maps client fd to client meta
		std::map<int, ClientMeta> _clientMap;

		void _setupAllListeners();
		void _setUpListener(int port);

		void _handleListenerEvents(const pollfd &listener);
		void _handleClientEvents(const pollfd &client);

		void _addClient(int listenerFd);
		void _removeClient(int fd);
		void _recvFromClient(int fd);
		bool _requestIsComplete(ClientMeta &client);
		void _sendToClient(int fd);

		void _addToPoll(int fd, short events, short revents);
		void _removeFromPoll(int i);
		void _addToClientMap(int fd, int port, int listenerFd);
		void _removeFromClientMap(int fd);

		bool _isClient(int fd);
		bool _isListener(int fd);

		static bool _noEvents(const pollfd &pollfd);
		static bool _clientIsConnecting(const pollfd &listener);
		static bool _clientIsDisconnected(const pollfd &client);
		static bool _clientIsSendingData(const pollfd &client);
		static bool _clientIsReadyToReceive(const pollfd &client);

		static void printError(std::string message);

	// exceptions
	public:

		class PollException : public std::exception
		{
			public:
				const char *what() const throw();
		};

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
