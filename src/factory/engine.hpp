#pragma once

#include "../entry.hpp"
#include "../window/MainWindow.hpp"

class Engine {
public:
	void StartUp( );

protected:
	MainWindow* wnd;
};

inline const auto g_engine = std::make_shared< Engine >( );