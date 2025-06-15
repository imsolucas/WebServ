# pragma once

# include "Http.h"
# include "Server.hpp"

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
			STATE_REQUEST_READY
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
			int listenerFd;
			int port;
			const Server *server;
			std::string requestBuffer;
			RequestMeta requestMeta;
			int errorCode;

			ClientMeta();
		};

		ClientManager(std::vector<pollfd> &poll, size_t &pollIndex, const std::vector<Server> &_servers);

		void addClient(int listenerFd, int port);
		void removeClient(int fd);
		void recvFromClient(int fd);
		void sendToClient(int fd);

		bool isClient(int fd);

		static const Location &matchURI(const std::string &URI, const std::vector<Location> &locations);
		static HttpResponse handleError(StatusCode code);
	private:
		// maps client fd to client meta
		std::map<int, ClientMeta> _clientMap;
		std::vector<pollfd> &_poll;
		size_t &_pollIndex;
		const std::vector<Server> &_servers;

		void _handleRequest(const ClientMeta &client);
		HttpResponse _buildResponse();

		void _addToClientMap(int fd, int listenerFd, int port);
		void _removeFromClientMap(int fd);

		void _handleIncomingData(int fd, const char *buffer, size_t bytesReceived);
		bool _headersAreComplete(ClientMeta &client);
		void _preparseHeaders(ClientMeta &client);
		void _determineBodyEnd(ClientMeta &client, const HttpRequest &req);
		const Server *_selectServerBlock(ClientMeta &client, const HttpRequest &req);
		bool _maxBodySizeExceeded(const ClientMeta &client);
		bool _bodyIsComplete(const ClientMeta &client);
};
