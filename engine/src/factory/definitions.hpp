#pragma once

#include "../entry.hpp"
#include <numbers>

constexpr int g_metres_in_cell = 2;
constexpr float g_cell_size = (float) g_metres_in_cell;
constexpr uint32_t k_cold_pool_amount = 5;
constexpr uint32_t k_pool_shift_period = 5;
constexpr uint32_t k_logic_updates_per_second = 30;
constexpr auto k_logic_update_interval = std::chrono::nanoseconds(1000000000 / k_logic_updates_per_second);


// simple math
namespace math {
	constexpr auto g_pi = std::numbers::pi_v< double >;// 3.14159265358979323846;
	constexpr auto g_pi2 = 2 * g_pi;

	// Точность сравнения вещетвенных чисел
	constexpr auto g_fepsilon = 0.0001f;

	bool fcompare(float x, float y);
}