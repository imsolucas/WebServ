# include "Clients.hpp"
# include "colors.h"
# include "utils.hpp"

# include <fcntl.h>
# include <fstream> // TODO: DELETE
# include <iostream>
# include <sys/socket.h> // accept, recv, send
# include <unistd.h> // close

using std::cout;
using std::string;

Clients::Clients(std::vector<pollfd> &poll, size_t &pollIndex) 
: _poll(poll), _pollIndex(pollIndex) {}

void Clients::addClient(int listenerFd, int port)
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

void Clients::removeClient(int fd)
{
	close(fd);
	utils::removeFromPoll(_poll, _pollIndex);
	_removeFromClientMap(fd);
	// decrement to prevent skipping the element that just moved into position i.
	_pollIndex--;
	cout << RED << "Client with fd " << fd << " disconnected!\n" << RESET;
}

// boolean reflects success of recv call().
void Clients::recvFromClient(int fd)
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
	{
		ClientMeta &client = _clientMap[fd];
		client.requestBuffer.append(buffer, bytesReceived);
		cout << "\nData received from client with fd " << fd <<  ":\n" << YELLOW << client.requestBuffer << "\n" << _RESET;
		if (requestIsComplete(client))
		{
			cout << "Received complete request from fd " << fd << ".\n";
			// change poll from POLLIN to POLLOUT to send response to client
			_poll[_pollIndex].events = POLLOUT;
		}
	}
}

// This function will check the http request headers for either the
// "Content-Length" or "Transfer-Encoding" field to determine when
// the http request has been fully received.
// GET requests are complete when the headers are received.
bool Clients::requestIsComplete(ClientMeta &client)
{
	// in a http request, the end of headers is demarcated by "\r\n\r\n".
	size_t headersEnd = client.requestBuffer.find("\r\n\r\n");
	// cout << "[DEBUG] Buffer size: " << client.requestBuffer.size() << ", headersEnd: " << headersEnd << "\n";
	// cout << "[DEBUG] Buffer: " << client.requestBuffer << "\n";

	// headersParsed check to ensure code block is only entered once.
	if (!client.headersParsed && headersEnd != string::npos)
	{
		client.headersParsed = true;
		string headers = client.requestBuffer.substr(0, headersEnd);

		// GET requests do not have a body, so they are complete when the headers are received.
		if (headers.find("GET ") == 0)
			return true;

		if (headers.find("Transfer-Encoding: chunked") != string::npos)
			client.chunkedRequest = true;
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
				if (!(ss >> client.contentLength))
					client.contentLength = 0;
			}
			else
				client.contentLength = 0;
		}
	}

	if (client.headersParsed)
	{
		if (client.chunkedRequest)
		{
			// a chunked request ends with "0\r\n\r\n".
			if (client.requestBuffer.find("0\r\n\r\n", headersEnd) != string::npos)
				return true;
		}
		else
		{
			// a non-chunked request is complete when the buffer can contain
			// the header length + "\r\n\r\n" + content length.
			if (client.requestBuffer.size() >= headersEnd + 4 + client.contentLength)
				return true;
		}
	}
	return false;
}

void Clients::sendToClient(int fd)
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

bool Clients::isClient(int fd)
{
	return _clientMap.count(fd);
}

// problematic/disconnected clients
// POLLERR = socket error (I/O error or connection reset or unusable socket)
// POLLHUP = hang up (client disconnected)
// POLLNVAL = invalid fd (fd closed but still in _poll list)
bool Clients::clientIsDisconnected(const pollfd &client)
{
	return client.revents & (POLLERR | POLLHUP | POLLNVAL);
}

// POLLIN on client means client is sending data to the server
bool Clients::clientIsSendingData(const pollfd &client)
{
	return client.revents & POLLIN;
}

// POLLOUT on client means client is ready to receive data
bool Clients::clientIsReadyToReceive(const pollfd &client)
{
	return client.revents & POLLOUT;
}

void Clients::_addToClientMap(int fd, int port, int listenerFd)
{
	ClientMeta meta;

	meta.port = port;
	meta.listenerFd = listenerFd;
	meta.requestBuffer = "";
	meta.headersParsed = false;
	meta.chunkedRequest = false;
	meta.contentLength = -1; // default to -1 to avoid confusion
	_clientMap[fd] = meta;
}

void Clients::_removeFromClientMap(int fd)
{
	_clientMap.erase(fd);
}
