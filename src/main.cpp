# include <iostream>

# include "colors.h"
# include "WebServer.hpp"

using std::cerr;

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cerr << RED + "Usage: ./webserv [configuration file]\n";
		return 1;
	}

	WebServer wb(argv[1]);
	wb.run();
}
