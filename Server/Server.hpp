# pragma once

# include <vector>

# include "Socket.hpp"

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
		Socket _listening;
		std::vector<Socket> _connected;
		ServerState _state;

		static void _respond(const Socket &client, const std::string &msg);

		void _listen();
};
