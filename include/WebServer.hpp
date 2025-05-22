# pragma once

# include <iostream>

# include "Config.hpp"

# include "colors.h"

# include <map>

# include <fcntl.h>
# include <iostream>
# include <netinet/in.h> // htons, sockaddr_in struct
# include <poll.h>
# include <sys/socket.h> // accept, bind, listen, recv, setsockopt, socket
# include <unistd.h> // close
# include <vector>

class WebServer
{
	public:
		WebServer(const std::string &config);
		~WebServer();

		void run();
		void closeAllSockets();

	private:

		Config _cfg;

		std::vector<pollfd> _poll;

		struct SocketMeta
		{
			enum Role { LISTENER, CLIENT };
			Role type;
			int listenerFd; // only for clients
			int port; // only for listeners
		};

		std::map<int, SocketMeta> _socketMap;

		void _init();
		void _parse(const std::string &config);

		void _removeClient(const pollfd & socket, int i);
		void _addClient(const pollfd & socket);
		bool _recvFromClient(const pollfd & socket, int i);
		void _sendResponse(const pollfd & socket, int i);

		void _setUpListener(int port);

		void _addToPoll(int fd, short events, short revents);
		void _removeFromPoll(int i);
		void _addToSocketMap(int fd, SocketMeta::Role type, int listenerFd, int port);
		void _removeFromSocketMap(int fd);

		bool _isClient(const SocketMeta & socketMeta) const;
		bool _isListener(const SocketMeta & socketMeta) const;
		bool _noEvents(const pollfd & socket) const;
		bool _clientIsDisconnected(const pollfd & socket, const SocketMeta & socketMeta) const;
		bool _clientIsConnecting(const pollfd & socket, const SocketMeta & socketMeta) const;
		bool _clientIsSendingData(const pollfd & socket, const SocketMeta & socketMeta) const;

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
				SocketCreationException(const std::string& msg);
		};

		class SocketConfigException : public std::runtime_error
		{
			public:
				SocketConfigException(const std::string& msg);
		};

		class SocketOptionException : public std::runtime_error
		{
			public:
				SocketOptionException(const std::string& msg);
		};

		class BindException : public std::runtime_error
		{
			public:
				BindException(const std::string& msg);
		};

		class ListenException : public std::runtime_error
		{
			public:
				ListenException(const std::string& msg);
		};
};
