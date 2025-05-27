# include <iostream>

# include "Includes.h"

using std::cerr;

void throwError(const std::string &message)
{
	cerr << BOLD << RED << "Error: " << message << RESET << std::endl;
	throw WebException(message);
}
using std::cout;

int main(int argc, char *argv[])
{
	try {
		if (argc < 1 || argc > 2) {
			std::cerr << BOLD << RED << "Usage: " << argv[0] << " [config_file_path]" << " or " << argv[0] << RESET << std::endl;
			return EXIT_FAILURE;
		}
		std::string configFile = (argc == 2) ? argv[1] : DEFAULT_CONFIG_PATH;
		WebServer wb(configFile);
	} catch (const std::exception &e) {
		cerr << BOLD << RED << "Exception: " << e.what() << RESET << std::endl;
		return EXIT_FAILURE;
	}

	// try
	// {
	// 	wb.run();
	// }
	// catch(const std::exception& e)
	// {
	// 	cerr << RED << "Error: " << e.what() << '\n' << RESET;
	// 	return 1;
	// }
	// return 0;
}
