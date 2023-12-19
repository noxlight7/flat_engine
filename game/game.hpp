#pragma once

#include <entry.hpp>
#include "factory/engine.hpp"

class Game : public Engine {
	//Game(const char* title, uint32_t width, uint32_t height)
	//	: Engine(title, width, height) {}
	void onInit();
	void onRender();
};

inline const auto g_game =
	std::make_shared<Game>("Game");