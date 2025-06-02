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

		struct SocketMeta
		{
			enum Role { LISTENER, CLIENT };
			Role type;
			int port;
			int listenerFd; // only for clients
			std::string requestBuffer; // only for clients
			bool headersParsed; // only for clients;
			bool chunkedRequest; // only for clients
			int contentLength; // only for clients;
		};

		std::map<int, SocketMeta> _socketMap;
		
		void _parse(const std::string &config);
		void _setupAllListeners();
		void _setUpListener(int port);

		void _closeAllSockets();

		void _removeClient(const pollfd &socket, int i);
		void _addClient(const pollfd &socket);
		bool _recvFromClient(const pollfd &socket, int i);
		bool _requestIsComplete(SocketMeta &client);
		bool _sendToClient(const pollfd &socket, int i);

		void _addToPoll(int fd, short events, short revents);
		void _removeFromPoll(int i);
		void _addToSocketMap(int fd, SocketMeta::Role type, int port, int listenerFd);
		void _removeFromSocketMap(int fd);

		static bool _isClient(const SocketMeta &socketMeta);
		static bool _isListener(const SocketMeta &socketMeta);
		static bool _noEvents(const pollfd &socket);
		static bool _clientIsDisconnected(const pollfd &socket, const SocketMeta &socketMeta);
		static bool _clientIsConnecting(const pollfd &socket, const SocketMeta &socketMeta);
		static bool _clientIsSendingData(const pollfd &socket, const SocketMeta &socketMeta);
		static bool _clientIsReadyToReceive(const pollfd &socket, const SocketMeta &socketMeta);

		static void printError(std::string message);

		void _debugPollAndSocketMap() const;

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
