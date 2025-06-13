# include "utils.hpp"
# include "colors.h"

# include <iostream> // cerr

using std::cerr;

void utils::printError(string message)
{
	cerr << RED << "Error: " << message << "\n" << RESET;
}

// function overload so that we can call printError(e.what())
void utils::printError(const char* message)
{
	cerr << RED << "Error: " << message << "\n" << RESET;
}
