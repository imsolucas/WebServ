# include <iostream>
# include <csignal>

# include "colors.h"
# include "test.hpp"

volatile sig_atomic_t gStopLoop = 0;

int main()
{
	TestSuite t;
	test_http(t);
	assertEqual("test", 1,2);
	t.run();
}
