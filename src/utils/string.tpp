# include <sstream>

# include "utils.hpp"

using std::ostringstream;
using std::string;

template<typename T>
string utils::toString(const T &any)
{
	ostringstream oss;

	oss << any;

	return oss.str();
}
