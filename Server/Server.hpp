# pragma once

# include <vector>

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
		Socket _listener;
		std::vector<Connection> _clients;
		ServerState _state;
};
