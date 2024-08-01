#pragma once

#include "main.hpp"
#include "input/input.hpp"
#include "physics/physics.hpp"
#include "factory/engine.hpp"


class Game : public Engine {
public:
	Game(const char* title, uint32_t width = 1024, uint32_t height = 768);
	void onInit();
	void onRender();
	Vector getPlayerKeyboardSpeedDirection();
	Vector getPlayer2KeyboardSpeedDirection();

protected:
	SpaceObject m_test_obj;
	SpaceObject m_test_obj2;
	//SpaceObject m_test_fly;
	//SpaceObject m_test_static_obj;
	//SpaceObject m_test_static_obj2;
	District m_district;
	void init();
};

inline const auto g_game =
	std::make_shared<Game>("Game");
