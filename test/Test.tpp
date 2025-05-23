# include "colors.h"
# include "Test.hpp"

# define DIVIDER "\n--------------------\n"

using std::string;
using std::cout;

template <typename Output>
void assertEqual(const string &msg, const Output &actual, const Output &expected)
{
	if (actual == expected)
		cout << "[PASS] " + msg + "\n";
	else
	{
		cout << "[FAIL] " + msg + "\n";
		cout << "Expected:"DIVIDER << expected << DIVIDER;
		cout << "Actual:"DIVIDER << actual << DIVIDER;
	}
}
