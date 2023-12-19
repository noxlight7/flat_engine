#pragma once

#include "../entry.hpp"

// simple math
namespace math {
	const float g_pi = 3.14159265358979323846f;
	const float g_pi2 = 2 * g_pi;

	template <typename _Ty>
	ALWAYS_INLINE _Ty clamp( const _Ty value, const _Ty min, const _Ty max ) {
		return std::max( min, std::min( value, max ) );
	}
}