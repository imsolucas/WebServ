# pragma once

# include <iostream>

template <typename Output>
void assertEqual(const std::string &msg, const Output &actual, const Output &expected);

void test_http();

# include "Test.tpp"
