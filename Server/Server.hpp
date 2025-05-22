# pragma once

# include <vector>
# include <netinet/in.h>

// Receiving and sending data between a single client
struct Socket
{
	int			fd;
	sockaddr_in	addr;
	socklen_t	len;

	Socket() : fd(0), len(sizeof(addr)) {}
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

		void _listen();
};
