#include "definitions.hpp"

namespace math {
	bool fcompare(float x, float y) {
		return fabsf(x - y) < g_fepsilon;
	}
}
