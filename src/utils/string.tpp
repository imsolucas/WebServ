# include <sstream>

# include "utils.hpp"

using std::string;
using std::ostringstream;

template<typename Input>
string utils::toString(const Input &any)
{
	ostringstream oss;

	oss << any;

	return oss.str();
}
