# include <iostream>

# include "colors.h"
# include "test.hpp"

int main()
{
	TestSuite t;
	test_serialize(t);
	test_deserialize(t);
	test_matchURI(t);
	test_handleError(t);
	t.run();
}
