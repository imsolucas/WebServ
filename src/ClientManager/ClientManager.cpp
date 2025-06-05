# include "ClientManager.hpp"
# include "colors.h"
# include "utils.hpp"

# include <fcntl.h>
# include <fstream> // TODO: DELETE
# include <iostream>
# include <sys/socket.h> // accept, recv, send
# include <unistd.h> // close

using std::cout;
using std::string;

ClientManager::ClientManager(std::vector<pollfd> &poll, size_t &pollIndex, const Config &cfg)
: _poll(poll), _pollIndex(pollIndex), _cfg(cfg) {}

void ClientManager::addClient(int listenerFd, int port)
{
	// create a socket for the client on our server.
	int clientFd = accept(listenerFd, NULL, NULL);
	if (clientFd < 0)
	{
		utils::printError("Failed to add client.");
		return;
	}
	// set the client's socket to be non-blocking.
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		utils::printError("Failed to set non-blocking mode for client with fd " + utils::toString(clientFd) + ".\n");
		close(clientFd);
		return;
	}
	utils::addToPoll(_poll, clientFd, POLLIN, 0);
	_addToClientMap(clientFd, port, listenerFd);
	cout << GREEN <<  "Client with fd " << clientFd << " connected on port " << port << "!\n" << RESET;
}

void ClientManager::removeClient(int fd)
{
	close(fd);
	utils::removeFromPoll(_poll, _pollIndex);
	_removeFromClientMap(fd);
	// decrement to prevent skipping the element that just moved into position i.
	_pollIndex--;
	cout << RED << "Client with fd " << fd << " disconnected!\n" << RESET;
}

// boolean reflects success of recv call().
void ClientManager::recvFromClient(int fd)
{
	char buffer[4096];
	// recv() returns the number of bytes read.
	// 0 flag because socket has already been set to be non-blocking.
	// signed size_t can be negative.
	ssize_t bytesReceived = recv(fd, buffer, sizeof(buffer), 0);
	// 0 bytes read is the client's EOF signal when it closes its connection.
	// a more reliable signal to detect for graceful closure than POLLHUP.
	if (bytesReceived == 0)
	{
		cout << "Client with fd " << fd << " has hung up.\n";
		removeClient(fd);
	}
	else if (bytesReceived < 0)
	{
		utils::printError("Failed to receive request from client with fd " + utils::toString(fd) + ".\n");
		removeClient(fd);
	}
	else
		_handleIncomingData(fd, buffer, bytesReceived);
}

void ClientManager::sendToClient(int fd)
{
	// TODO: DELETE - ONLY FOR TESTING PURPOSES
	// -----------------------------------------------------------------------------------
	std::ifstream page("public/upload.html");
	string line, body;

	while (std::getline(page, line))
	body += line + "\n";

	// attempt to send a http response.
	// Modern browsers will reuse persistent connections due to HTTP/1.1 keep-alive, which is on by default.
	// So removing "Connection: close" will cause the browser to reuse the same client fd even across
	// different tabs.
	string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + utils::toString(body.size()) + "\r\n"
		"Connection: close\r\n"
		"\r\n" +
		body;
	// -----------------------------------------------------------------------------------

	// MSG_NOSIGNAL flag prevents SIGPIPE if the client has already closed the connection.
	// Often used in server code to avoid crashes from broken pipes e.g. when client
	// has closed their connection.
	if (send(fd, response.c_str(), response.size(), MSG_NOSIGNAL) <= 0)
		utils::printError("Failed to send response to client with fd " + utils::toString(fd) + ".\n");
	// remove client regardless whether send succeeded or failed.
	removeClient(fd);
}

bool ClientManager::isClient(int fd)
{
	return _clientMap.count(fd);
}

void ClientManager::_addToClientMap(int fd, int port, int listenerFd)
{
	ClientMeta meta;

	meta.state = STATE_INIT;
	meta.port = port;
	meta.listenerFd = listenerFd;
	meta.requestBuffer = "";
	meta.server = NULL;

	// default to -1 to prevent confusion
	meta.requestMeta.headersEnd = -1;
	meta.requestMeta.chunkedRequest = false;
	meta.requestMeta.contentLength = -1;

	_clientMap[fd] = meta;
}

void ClientManager::_removeFromClientMap(int fd)
{
	_clientMap.erase(fd);
}

void ClientManager::_handleIncomingData(int fd, const char *buffer, size_t bytesReceived)
{
	ClientMeta &client = _clientMap[fd];

	client.requestBuffer.append(buffer, bytesReceived);
	cout << "\nData received from client with fd " << fd <<  ":\n" << YELLOW << client.requestBuffer << "\n" << _RESET;

	switch (client.state)
	{
		case STATE_INIT:
			client.state = STATE_RECVING;
			// fallthrough on purpose

		case STATE_RECVING:
		{
			if (_headersAreComplete(client))
				_preparseHeaders(client);
			else
				break;
			// fallthrough on purpose
		}

		case STATE_HEADERS_PREPARSED:
		{
			if (_bodyIsComplete(client))
				client.state = STATE_REQUEST_READY;
			else
				break;
			// fallthrough on purpose
		}

		case STATE_REQUEST_READY:
		{
			cout << "Received complete request from fd " << fd << ".\n";
			// change poll from POLLIN to POLLOUT to send response to client
			_poll[_pollIndex].events = POLLOUT;
			break;
		}

		default:
			break;
	}
}

bool ClientManager::_headersAreComplete(ClientMeta &client)
{
	client.requestMeta.headersEnd = client.requestBuffer.find("\r\n\r\n");
	// in a http request, the end of headers is demarcated by "\r\n\r\n".
	return client.requestMeta.headersEnd != string::npos;
}

void ClientManager::_preparseHeaders(ClientMeta &client)
{
	string headers = client.requestBuffer.substr(0, client.requestMeta.headersEnd);
	_determineBodyEnd(client, headers);
	client.state = STATE_HEADERS_PREPARSED;
}

// This function will check the http request headers for either the
// "Content-Length" or "Transfer-Encoding" field to determine when
// the http request has been fully received.
// GET requests are complete when the headers are received.
void ClientManager::_determineBodyEnd(ClientMeta &client, string headers)
{
	if (headers.find("GET ") == 0)
	{
		client.requestMeta.contentLength = 0;
		return;
	}

	if (headers.find("Transfer-Encoding: chunked") != string::npos)
		client.requestMeta.chunkedRequest = true;
	else
	{
		string nonChunkedMarker = "Content-Length:";
		size_t pos = headers.find(nonChunkedMarker);
		if (pos != string::npos)
		{
			pos += nonChunkedMarker.size();
			// create a stringstream to extract the content length as an integer
			std::stringstream ss(headers.substr(pos));
			// set content length to 0 if fail to parse a valid number
			if (!(ss >> client.requestMeta.contentLength))
				client.requestMeta.contentLength = 0;
		}
		else
			client.requestMeta.contentLength = 0;
	}
}

bool ClientManager::_bodyIsComplete(const ClientMeta &client)
{
	const size_t headersEnd = client.requestMeta.headersEnd;
	if (client.requestMeta.chunkedRequest)
		// a chunked request ends with "0\r\n\r\n".
		return (client.requestBuffer.find("0\r\n\r\n", headersEnd) != string::npos);
	else
		// a non-chunked request is complete when the buffer can contain
		// the header length + "\r\n\r\n" + content length.
		return (client.requestBuffer.size() >= headersEnd + 4 + client.requestMeta.contentLength);
}
