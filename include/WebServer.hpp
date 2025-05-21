# pragma once

# include <iostream>

# include "Config.hpp"

# include "colors.h"

# include <map>

// moved from Listener.hpp
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

	private:
		struct connectionMeta
		{
			enum type { LISTENER, CLIENT };
			type type;
			int listenerFd; // only for clients
			int port; // only for listeners
		};

		Config _cfg;
		std::map<int, connectionMeta> fdMap;
		std::vector<pollfd> _fds;

		void _init();
		void _parse(const std::string &config);

		void _setUpListener(int port);
		void _addToPoll(int fd, short events, short revents);

		void _sendResponse(int fd);

	// exceptions
	public:
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
