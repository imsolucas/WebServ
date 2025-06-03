# pragma once

# include <map>
# include <poll.h>
# include <string>
# include <vector>

class Clients
{	
	public:
		struct ClientMeta
		{
			int port;
			int listenerFd;
			std::string requestBuffer;
			bool headersParsed;
			bool chunkedRequest;
			int contentLength;
		};

		Clients(std::vector<pollfd> &poll, size_t &pollIndex); 

		void addClient(int listenerFd, int port);
		void removeClient(int fd);
		void recvFromClient(int fd);
		bool requestIsComplete(ClientMeta &client);
		void sendToClient(int fd);

		bool isClient(int fd);
		static bool clientIsDisconnected(const pollfd &client);
		static bool clientIsSendingData(const pollfd &client);
		static bool clientIsReadyToReceive(const pollfd &client);

	private:
		// maps client fd to client meta
		std::map<int, ClientMeta> _clientMap;
		std::vector<pollfd> &_poll;
		size_t &_pollIndex;

		void _addToClientMap(int fd, int port, int listenerFd);
		void _removeFromClientMap(int fd);

};
