# pragma once

# include <vector>
# include <netinet/in.h>

// Receiving and sending data between a single client
struct Socket
{
	int			fd;
	sockaddr_in	addr;
	socklen_t	len;

	Socket();
};

// This is a TCP server
class Server
{
	public:
		void run();
		void init();

	private:
		Socket _listening;
		std::vector<Socket> _connected;

		static void _respond(const Socket &client, const std::string &msg);
		
		void _listen();
};
