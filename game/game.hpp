#pragma once

#include <entry.hpp>
#include "factory/engine.hpp"

class Game : public Engine {
public:
	Game(const char* title, uint32_t width = 1024, uint32_t height = 768);
	void onInit();
	void onRender();
};

inline const auto g_game =
	std::make_shared<Game>("Game");