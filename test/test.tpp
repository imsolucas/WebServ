# include "colors.h"
# include "test.hpp"

using std::string;
using std::cerr;

// * `==` & `<<` operator overload is required for class/struct objects
// ! this function does not work with pointers
template <typename Output>
bool assertEqual(const string &msg, const Output &actual, const Output &expected)
{
	if (actual == expected)
	{
		cerr << "[" + BG_GREEN + "PASS" + RESET + "] " + BOLD + msg + "\n" + RESET;
		return true;
	}
	else
	{
		cerr << "[" + BG_RED + "FAIL" + RESET + "] " + BOLD + msg + "\n" + RESET;
		cerr << DIVIDER_THIN + YELLOW + " Expected " + DIVIDER_THIN"\n" + RESET;
		cerr << expected << "\n";
		cerr << DIVIDER_THIN + YELLOW + " Actual " + DIVIDER_THIN"\n" + RESET;
		cerr << actual << "\n";
		return false;
	}
}
