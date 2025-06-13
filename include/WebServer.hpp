# pragma once

# include "Server.hpp"
# include "Location.hpp"
# include "ListenerManager.hpp"
# include "ClientManager.hpp"
# include "Http.h"

# include <sstream>
# include <stdexcept>
# include <fstream>
# include <map>
# include <poll.h>
# include <vector>

class WebServer
{
	public:
		WebServer(const std::string &config);
		~WebServer();

		void run();

		std::vector<Server> getServers() const;
		void printTokens(const std::vector<std::string> &tokens) const;

		std::vector<std::string> tokenize(const std::string &str);

		static const Server &matchServer(const std::string &host, const std::vector<Server> &servers);
		static const Location &matchURI(const std::string &URI, const std::vector<Location> &locations);
		static HttpResponse handleError(StatusCode code);

	private:
		// Config _cfg;
		std::vector<Server> _servers;
		std::vector<pollfd> _poll;
		ListenerManager _listenerManager;
		ClientManager _clientManager;

		size_t _pollIndex;

		void _handleRequest(const HttpRequest &request);
		HttpResponse _buildResponse();

		void _handleListenerEvents(const pollfd &listener);
		void _handleClientEvents(const pollfd &client);

		static bool _noEvents(const pollfd &pollfd);
		static bool _clientIsConnecting(const pollfd &listener);
		static bool _clientIsDisconnected(const pollfd &client);
		static bool _clientIsSendingData(const pollfd &client);
		static bool _clientIsReadyToReceive(const pollfd &client);

		std::vector<Server> _parseTokens(const std::vector<std::string> &tokens);
		std::vector<Server> _parseConfig(const std::string &filePath);
		Server parseServerBlock(const std::vector<std::string> &tokens, size_t &i);
		Location parseLocationBlock(const std::vector<std::string> &tokens, size_t &i);

	public:
		class PollException : public std::runtime_error
		{
			public:
				PollException();
		};
};
