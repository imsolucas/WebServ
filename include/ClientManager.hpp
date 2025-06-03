# pragma once

# include <map>
# include <poll.h>
# include <string>

class ClientManager
{
	private:
		struct ClientMeta
		{
			int port;
			int listenerFd;
			std::string requestBuffer;
			bool headersParsed;
			bool chunkedRequest;
			int contentLength;
		};
		// maps client fd to client meta
		std::map<int, ClientMeta> _clientMap;
		std::vector<pollfd> &_poll;
		size_t &_pollIndex;

	public:
		ClientManager(std::vector<pollfd> _poll);

		void addClient(int listenerFd, int port);
		void removeClient(int fd);
		void recvFromClient(int fd);
		bool requestIsComplete(ClientMeta &client);
		void sendToClient(int fd);

		void addToClientMap(int fd, int port, int listenerFd);
		void removeFromClientMap(int fd);

		bool isClient(int fd);
		static bool clientIsDisconnected(const pollfd &client);
		static bool clientIsSendingData(const pollfd &client);
		static bool clientIsReadyToReceive(const pollfd &client);
}
