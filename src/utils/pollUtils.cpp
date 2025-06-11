# include "utils.hpp"

// add a file descriptor and its event to be monitored by poll().
// short is a bitmask for the events to monitor.
void utils::addToPoll(std::vector<pollfd> &poll, int fd, short events, short revents)
{
	// A pollfd struct is used to monitor file descriptors for events:
	// 1) FD to monitor
	// 2) EVENTS to monitor (POLLIN = incoming data, POLLOUT = outgoing data)
	//    In case of error, poll() will set error flags in the revents field.
	//    server POLLIN - client wishing to connect
	//    client POLLIN - client wishing to send data
	// 3) REVENTS (returned events which will be set by poll())
	struct pollfd pfd;

	pfd.fd = fd;
	pfd.events = events;
	pfd.revents = revents;
	poll.push_back(pfd);
}

void utils::removeFromPoll(std::vector<pollfd> &poll, int i)
{
	poll.erase(poll.begin() + i);
}
