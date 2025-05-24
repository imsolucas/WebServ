# include <iostream>

# include "colors.h"
# include "test.hpp"

int main()
{
	TestSuite t;
	test_http(t);
	t.run();
}
