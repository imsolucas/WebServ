# pragma once

# include <csignal>

// extern works like a forward declaration so that compiler references the variable
// and does not re-allocate it.
extern volatile sig_atomic_t gStopLoop;

void handleSignal(int signum);
