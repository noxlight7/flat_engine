#include "game.hpp"
#include "chrono"
#include "utils/hash.hpp"
#include "physics/collisions.hpp"
#include "display/camera.hpp"
#include "generators/so_gens.hpp"
#include <serializer.hpp>

GameEngine::GameEngine(boost::asio::io_context& io_context, std::string host, uint16_t port)
	: ClientEngine(io_context, std::move(host), port), m_district(100, 100),
      m_test_obj(true, ObjectForm(0.8f, 0.8f)),
      m_test_obj2(true, ObjectForm(1.f, 1.2f))
{
	ClientEngine::init(k_wnd_title, k_wnd_width, k_wnd_height);
	m_district.setRenderer(k_wnd_width, k_wnd_height);
	this->setWorld(m_district.getRenderer());
	m_test_obj.addDrawInfo(g_textures->get("assets/sprites/test.png"));
	m_test_obj2.addDrawInfo(g_textures->get("assets/sprites/test2.png"));
}

void GameEngine::onInit() {
	m_test_obj.moveTo(&m_district, 90, 90);
	m_test_obj2.moveTo(&m_district, 87, 87);
	m_district.getRenderer()->setCameraHeight(2.5f);
	SpaceObjectGenerator::generateSpaceObjects(&m_district, 0.123f,
		g_textures->get("assets/sprites/test2.png"));
}

void GameEngine::onRender() {
	// Сюда добавить код, выполняющийся
	// перед обработкой кадра 
	static auto start_time = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point x;

	auto current_time = std::chrono::high_resolution_clock::now();
	static auto undo_time = start_time;
	float dt = std::chrono::duration<float, std::chrono::seconds::period>
		(current_time - undo_time).count();
	undo_time = current_time;

	float ct = std::chrono::duration<float, std::chrono::seconds::period>
		(start_time - current_time).count();

	Vector v = getPlayerKeyboardSpeedDirection();

	m_test_obj.setSpeedDirection(v);
	m_test_obj.setCurrentSpeed(v.x != 0 || v.y != 0 ? 8.f: 0.f);

	Vector v2 = getPlayer2KeyboardSpeedDirection();

	m_test_obj2.setSpeedDirection(v2);
	m_test_obj2.setCurrentSpeed(v2.x != 0 || v2.y != 0 ? 5.27f : 0.f);

	g_camera->setOrigin(m_test_obj.getPosition());

	m_district.moveObjects(dt);

	// m_session->sendPacket(flat_engine::network::SHARED_TEST_TEXT_MESSAGE,
	// 	flat_engine::network::Serializer::serializeTextMessage("123"));
	m_session->sendPacket(flat_engine::network::SERVER_SEND_PLAYER_AVATAR,
		flat_engine::network::Serializer::serializePlayerAvatar(
			m_test_obj.getPosition().m_coords.x, m_test_obj.getPosition().m_coords.y,
			m_test_obj.getPosition().m_index.x, m_test_obj.getPosition().m_index.y,
			v.x, v.y));
}

std::vector<std::string> GameEngine::getAllTexturesFullNames() {
	return {
		"assets/sprites/test.png",
		"assets/sprites/test2.png",
		"assets/sprites/circle_move.png",
		"assets/sprites/circle_static.png"
	};
}

Vector GameEngine::getPlayerKeyboardSpeedDirection() {
	Vector v_direct{};

	if (isKeyPressed(ScanCode::KEY_W))
		v_direct.y = 1;
	if (isKeyPressed(ScanCode::KEY_S))
		v_direct.y = -1;
	if (isKeyPressed(ScanCode::KEY_D))
		v_direct.x = 1;
	if (isKeyPressed(ScanCode::KEY_A))
		v_direct.x = -1;

	return v_direct;
}

Vector GameEngine::getPlayer2KeyboardSpeedDirection() {
	Vector v_direct{};

	if (isKeyPressed(ScanCode::KEY_UP))
		v_direct.y = 1;
	if (isKeyPressed(ScanCode::KEY_DOWN))
		v_direct.y = -1;
	if (isKeyPressed(ScanCode::KEY_RIGHT))
		v_direct.x = 1;
	if (isKeyPressed(ScanCode::KEY_LEFT))
		v_direct.x = -1;

	return v_direct;
}

void GameEngine::init() {
}
