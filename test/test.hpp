# pragma once

# include <iostream>

# include "colors.h"

# define FAIL RED
# define PASS GREEN

void test(void (*t)());
void http();

void printHeader(const std::string &msg);
