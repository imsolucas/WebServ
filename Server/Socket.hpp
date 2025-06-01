# pragma once

# include <string.h>
# include <netinet/in.h>

// Receiving and sending data between a single client
struct Socket
{
	int			fd;
	sockaddr_in	addr;
	socklen_t	len;

	Socket() : fd(0), len(sizeof(addr)) { memset(&addr, 0, len); }
};
