# pragma once

# include "Config.hpp"
# include "Http.h"

# include <map>
# include <poll.h>
# include <string>
# include <vector>

class ClientManager
{
	public:
		enum ClientState
		{
			STATE_INIT,
			STATE_RECVING,
			STATE_HEADERS_PREPARSED,
			STATE_REQUEST_READY,
			STATE_RESPONSE_READY,
			STATE_ERROR_413
		};

		struct RequestMeta
		{
			size_t headersEnd;
			bool chunkedRequest;
			int contentLength;
		};

		struct ClientMeta
		{
			ClientState state;
			int port;
			int listenerFd;
			std::string requestBuffer;
			const Server *server;

			RequestMeta requestMeta;
		};

		ClientManager(std::vector<pollfd> &poll, size_t &pollIndex, const Config &cfg);

		void addClient(int listenerFd, int port);
		void removeClient(int fd);
		void recvFromClient(int fd);
		void sendToClient(int fd);

		bool isClient(int fd);

	private:
		// maps client fd to client meta
		std::map<int, ClientMeta> _clientMap;
		std::vector<pollfd> &_poll;
		size_t &_pollIndex;
		const Config &_cfg;

		void _addToClientMap(int fd, int port, int listenerFd);
		void _removeFromClientMap(int fd);

		void _handleIncomingData(int fd, const char *buffer, size_t bytesReceived);
		bool _headersAreComplete(ClientMeta &client);
		void _preparseHeaders(ClientMeta &client);
		void _determineBodyEnd(ClientMeta &client, HttpRequest req);
		bool _bodyIsComplete(const ClientMeta &client);

};
