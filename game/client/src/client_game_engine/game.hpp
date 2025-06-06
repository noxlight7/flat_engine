#pragma once

#include "routes.hpp"
#include "../main.hpp"
#include "input/input.hpp"
#include "physics/physics.hpp"
#include "client/client_engine.hpp"
#include "display/district_renderer.hpp"

#ifdef _WIN32
const std::string k_wnd_title = "GameEngine";
#else
constexpr std::string k_wnd_title = "GameEngine";
#endif
constexpr float k_wnd_width = 1024;
constexpr float k_wnd_height = 768;

class GameEngine : public ClientEngine {
public:

	static GameEngine& instance() {
		static GameEngine engine = GameEngine(
			flat_engine::network::k_host_name,
			flat_engine::network::k_host_tcp_port,
			flat_engine::network::k_host_udp_port);
		return engine;
	}

	void onInit() final;
	void initTerrains();
	void onLogicUpdate() final;
	void initDisplayObjects() final;
	void onKeyDown(int key, int scancode, int action, int mods) override;
	void onRender() override;
	Vector getPlayerKeyboardSpeedDirection();
	Vector getPlayer2KeyboardSpeedDirection();

protected:
	GameEngine(std::string host, uint16_t port, uint16_t udp_port);
	~GameEngine() override = default;
	SpaceObject m_test_obj;
	SpaceObject m_test_obj2;
	District m_district;
	DistrictRenderer m_district_renderer;
	void init();
};
