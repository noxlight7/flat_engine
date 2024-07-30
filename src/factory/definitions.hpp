#pragma once

#include "../entry.hpp"
#include <numbers>

constexpr int g_metres_in_cell = 2;
constexpr float g_cell_size = (float) g_metres_in_cell;

// simple math
namespace math {
	constexpr auto g_pi = std::numbers::pi_v< double >;// 3.14159265358979323846;
	constexpr auto g_pi2 = 2 * g_pi;

	// “очность сравнени€ вещетвенных чисел
	constexpr auto g_fepsilon = 0.0001f;

	bool fcompare(float x, float y);
}