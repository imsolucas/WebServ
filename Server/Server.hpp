# pragma once

# include <map>
# include <sys/epoll.h>

# include "Connection.hpp"

enum ServerState
{
	RUNNING,
	TERMINATE
};

// This is a TCP server
class Server
{
	public:
		Server();
		~Server();

		void run();

	private:
		int _epollInstance;
		epoll_event events[16];
		Socket _listener;
		std::map<int, Connection*> _clients;
		ServerState _state;

		void _handleEvents();
		void _handleIncomingConnection();
		void _handleRequest();

		void _receiveData(Connection *client);
		void _sendData(Connection *client);
		void _establishConnection();
		void _closeConnection(Connection *client);
		void _registerEvents(int fd, uint32_t events);
};
