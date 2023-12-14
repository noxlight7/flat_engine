#pragma once

#include "../entry.hpp"

class Engine {
public:
	void StartUp( );
};

inline const auto g_engine = std::make_shared< Engine >( );