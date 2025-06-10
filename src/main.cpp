# include <iostream>

# include "Includes.h"

using std::cerr;
using std::cout;

int main(int argc, char *argv[])
{
	signal(SIGINT, handleSignal);

	if (argc < 1 || argc > 2)
	{
		std::cerr << BOLD << RED << "Usage: " << argv[0] << " [config_file_path]" << " or " << argv[0] << RESET << std::endl;
		return EXIT_FAILURE;
	}
	std::string configFile = (argc == 2) ? argv[1] : DEFAULT_CONFIG_PATH;

	try
	{
		WebServer wb(configFile);
		wb.run();
	}
	catch(const std::exception& e)
	{
		utils::printError(e.what());
		return 1;
	}
	return 0;
}
