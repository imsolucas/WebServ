# pragma once

# include <iostream>

# define DIVIDER_THICK "\n"_BG_BLUE"=============================="_RESET"\n"
# define DIVIDER_THIN "\n"_BG_CYAN"------------------------------"_RESET"\n"

template <typename Output>
void assertEqual(const std::string &msg, const Output &actual, const Output &expected);

void printHeader(const std::string &msg);

void test_http();

# include "test.tpp"
