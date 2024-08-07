#pragma once

#include "../../client/src/main.hpp"
#include "input/input.hpp"
#include "physics/physics.hpp"
#include "factory/engine.hpp"


class GameEngine : public Engine {
public:
	GameEngine(const char* title, uint32_t width = 1024, uint32_t height = 768);
	void onInit();
	void onRender();
	Vector getPlayerKeyboardSpeedDirection();
	Vector getPlayer2KeyboardSpeedDirection();

protected:
	SpaceObject m_test_obj;
	SpaceObject m_test_obj2;
	District m_district;
	void init();
};

inline const auto g_game =
	std::make_shared<GameEngine>("GameEngine");
