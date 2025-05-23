# include "colors.h"
# include "Test.hpp"

using std::string;
using std::cout;

// * `==` operator overload is required for class/struct objects
template <typename Output>
void assertEqual(const string &msg, const Output &actual, const Output &expected)
{
	if (actual == expected)
	{
		cout << "[" << BG_GREEN << "PASS" << RESET << "] "
			<< msg << RESET << "\n";
	}
	else
	{
		cout << "[" + BG_RED + "FAIL" + RESET + "] ";
		cout << msg + RESET + "\n";
		cout << YELLOW + "Expected:" + RESET;
		cout << DIVIDER_THIN;
		cout << RESET << expected << DIVIDER_THIN;
		cout << YELLOW + "Actual:" + RESET;
		cout << DIVIDER_THIN;
		cout << RESET << actual << DIVIDER_THIN;
	}
}
