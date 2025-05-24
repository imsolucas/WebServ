# include <iostream>
# include <csignal>

# include "colors.h"
# include "test.hpp"

using std::cout;

volatile sig_atomic_t gStopLoop = 0;

int main()
{
	test_http();
}
