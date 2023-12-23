#pragma once

#include "../entry.hpp"

// simple math
namespace math {
	constexpr double long g_pi = 3.14159265358979323846;
	constexpr double long g_pi2 = 2 * g_pi;

	// “очность сравнени€ вещетвенных чисел
	constexpr float g_fepsilon = 0.0001f;

	bool fcompare(float x, float y);
}