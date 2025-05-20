# include <iostream>

# include "Includes.h"
# include "WebServer.hpp"

using std::cerr;

void throwError(const std::string &message)
{
	cerr << BOLD << RED << "Error: " << message << RESET << std::endl;
	throw WebException(message);
}

int main(int argc, char *argv[])
{
	try {
		if (argc < 1 || argc > 2) {
			throwError("Usage: ./webserv [config_file] or ./webserv");
		}
		std::string configFile = (argc == 2) ? argv[1] : DEFAULT_CONFIG_PATH;
	} catch (const std::exception &e) {
		cerr << BOLD << RED << "Exception: " << e.what() << RESET << std::endl;
		return EXIT_FAILURE;
	}
}
