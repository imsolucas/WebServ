# pragma once

# include <iostream>

# include "Config.hpp"

# include "colors.h"

# include <map>

# include <fcntl.h> // fcntl
# include <iostream>
# include <netinet/in.h> // htons, sockaddr_in struct
# include <poll.h>
# include <sys/socket.h> // bind, listen, recv, socket
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
		bool _readFromClient(const pollfd & socket, int i);
		void _sendResponse(int fd);

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

		void _debugPollAndSocketMap() const;


	// exceptions
	public:

		class PollException : public std::exception
		{
			public:
				const char *what() const throw();
		};


		class WebServerException : public std::exception
		{
			protected:
				std::string _message;

			public:
				WebServerException(std::string err);
				virtual ~WebServerException() throw();
				const char *what() const throw();
		};

		class SocketCreationException : public WebServerException
		{
			public:
				SocketCreationException(std::string err);
		};
};
