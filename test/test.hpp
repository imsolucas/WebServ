# pragma once

# include <iostream>
# include <vector>

# define DIVIDER_THICK_BG "\n" _BG_BLUE "==============================" _RESET "\n"
# define DIVIDER_THIN_BG "\n" _BG_CYAN "------------------------------" _RESET "\n"

# define DIVIDER_THICK _BLUE "===============================" _RESET
# define DIVIDER_NORMAL _CYAN "-------------------------------" _RESET
# define DIVIDER_THIN _CYAN "- - - - - - - - - - - - - - - -" _RESET

class TestSuite
{
	public:
		TestSuite();

		void addTest(bool (*t)());
		void run();

	private:
		std::vector<bool (*)()> _tests;
		unsigned int _passes;
		unsigned int _fails;
};

template <typename Output>
bool assertEqual(const std::string &msg, const Output &actual, const Output &expected);

void printHeader(const std::string &msg);
void printSummary(unsigned int tests, unsigned int passes, unsigned int fails);

void test_serialize(TestSuite &t);
void test_deserialize(TestSuite &t);
void test_matchURI(TestSuite &t);
void test_handleError(TestSuite &t);
void test_buildResponse(TestSuite &t);

# include "test.tpp"
