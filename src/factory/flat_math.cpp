#include "definitions.hpp"

namespace math {
	bool fcompare(float x, float y) {
		return abs(x - y) < g_fepsilon;
	}
}
