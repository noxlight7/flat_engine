#pragma once

#include "routes.hpp"
#include "../main.hpp"
#include "input/input.hpp"
#include "physics/physics.hpp"
#include "client/client_engine.hpp"

constexpr std::string k_wnd_title = "GameEngine";
constexpr float k_wnd_width = 1024;
constexpr float k_wnd_height = 768;

class GameEngine : public ClientEngine {
public:

	static GameEngine& instance() {
		static boost::asio::io_context io_context;
		static GameEngine engine = GameEngine(
			io_context,
			flat_engine::network::k_host_name,
			flat_engine::network::k_host_tcp_port);
		return engine;
	}

	void onInit() final;
	void onRender() final;
	std::vector<std::string> getAllTexturesFullNames() final;
	Vector getPlayerKeyboardSpeedDirection();
	Vector getPlayer2KeyboardSpeedDirection();

protected:
	GameEngine(boost::asio::io_context& io_context, std::string host, uint16_t port);
	~GameEngine() override = default;
	SpaceObject m_test_obj;
	SpaceObject m_test_obj2;
	District m_district;
	void init();
};
