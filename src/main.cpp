# include "colors.h"
# include "signal.hpp"
# include "utils.hpp"
# include "WebServer.hpp"

# define DEFAULT_CONFIG_PATH "config/default.conf"

using std::cerr;
using std::endl;
using std::exception;
using std::string;

int main(int argc, char *argv[])
{
	signal(SIGINT, handleSignal); // CTRL-C
	signal(SIGTERM, handleSignal); // kill <pid>

	if (argc < 1 || argc > 2)
	{
		cerr << BOLD << RED << "Usage: " << argv[0] << " [config_file_path]" << " or " << argv[0] << RESET << endl;
		return 1;
	}
	string configFile = (argc == 2) ? argv[1] : DEFAULT_CONFIG_PATH;

	try
	{
		WebServer wb(configFile);
		wb.run();
	}
	catch(const exception& e)
	{
		utils::printError(e.what());
		return 1;
	}
	return 0;
}
