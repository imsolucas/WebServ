# include "signal.hpp"

# include <iostream>

using std::cout;

// volatie - prevents compiler optimizations that assume value won't change unexpectedly
// sig_atomic_t - a special integer type that guarantees atomic read and write
volatile sig_atomic_t gStopLoop = 0;

void handleSignal(int signum)
{
	if (signum == SIGINT)
		gStopLoop = 1;
	cout << "\nReceived SIGINT signal.\n";
}
