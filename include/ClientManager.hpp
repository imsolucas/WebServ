# pragma once

# include "Server.hpp"

# include <map>
# include <poll.h>
# include <string>
# include <vector>

class ClientManager
{
	public:
		ClientManager(std::vector<int> &pollRemoveQueue, std::vector<int> &pollToggleQueue,
						std::vector<int> &pollAddQueue, const std::vector<Server> &servers);

		void addClient(int listenerFd, int port);
		void removeClient(int fd);
		void recvFromClient(int fd);
		void sendToClient(int fd);

		bool isClient(int fd);

	private:
		enum ClientState
		{
			STATE_INIT,
			STATE_RECVING,
			STATE_HEADERS_PREPARSED,
			STATE_REQUEST_READY
		};

		struct RequestMeta
		{
			size_t headersEnd;
			bool chunkedRequest;
			int contentLength;
			bool maxBodySizeExceeded;
		};

		struct ClientMeta
		{
			ClientState state;
			int listenerFd;
			int port;
			const Server *server;
			std::string requestBuffer;
			RequestMeta requestMeta;

			ClientMeta();
		};

		struct PreparsedRequest
		{
			std::string method;
			std::map<std::string, std::string> headers;

			PreparsedRequest();
		};

		// maps client fd to client meta
		std::map<int, ClientMeta> _clientMap;
		std::vector<int> &_pollRemoveQueue;
		std::vector<int> &_pollToggleQueue;
		std::vector<int> &_pollAddQueue;
		const std::vector<Server> &_servers;

		void _resetClientMeta(int fd);
		void _addToClientMap(int fd, int listenerFd, int port);
		void _removeFromClientMap(int fd);

		void _handleIncomingData(int fd, const char *buffer, size_t bytesReceived);
		bool _headersAreComplete(ClientMeta &client);
		bool _preparseHeaders(ClientMeta &client);
		void _determineBodyEnd(ClientMeta &client, const PreparsedRequest &req);
		const Server *_selectServerBlock(ClientMeta &client, const PreparsedRequest &req);
		bool _maxBodySizeExceeded(const ClientMeta &client);
		bool _bodyIsComplete(const ClientMeta &client);

		std::string _handleRequest(const ClientMeta &client);
};
