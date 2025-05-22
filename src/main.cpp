# include <iostream>

# include "colors.h"
# include "signal.hpp"
# include "WebServer.hpp"

using std::cerr;
using std::cout;

// volatie - prevents compiler optimizations that assume value won't change unexpectedly
// sig_atomic_t - a special integer type that guarantees atomic read and write
volatile sig_atomic_t gStopLoop = 0;

void handle_signal(int signum)
{
	if (signum == SIGINT)
		gStopLoop = 1;
	cout << "\nReceived SIGINT signal.\n";
}

int main(int argc, char *argv[])
{

	signal(SIGINT, handle_signal);

	if (argc != 2)
	{
		cerr << RED + "Usage: ./webserv [configuration file]\n";
		return 1;
	}

	WebServer wb(argv[1]);
	try
	{
		wb.run();
	}
	catch(const std::exception& e)
	{
		wb.closeAllSockets();
		cerr << RED << "Error: " << e.what() << '\n' << RESET;
		return 1;
	}
	return 0;
}
