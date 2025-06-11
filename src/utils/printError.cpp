# include "utils.hpp"
# include "colors.h"

# include <iostream> // cerr

using std::cerr;

void utils::printError(string message)
{
	cerr << RED << "Error: " << message << RESET;
}
