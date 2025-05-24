# include <iostream>

# include "colors.h"
# include "signal.hpp"
# include "WebServer.hpp"

using std::cerr;
using std::cout;

int main(int argc, char *argv[])
{
	signal(SIGINT, handleSignal);

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
		cerr << RED << "Error: " << e.what() << '\n' << RESET;
		return 1;
	}
	return 0;
}
