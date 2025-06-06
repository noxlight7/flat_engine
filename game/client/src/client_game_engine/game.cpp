#include "game.hpp"
#include "chrono"
#include "utils/hash.hpp"
#include "physics/collisions.hpp"
#include "display/game_camera.hpp"
#include "generators/so_gens.hpp"
#include <shared_serializer.hpp>

#include "shared_constants.hpp"
#include "generators/terrain_gens.hpp"

auto form1 = ObjectForm(0.8f, 0.8f);
auto form2 = ObjectForm(0.8f, 1.2f);

GameEngine::GameEngine(std::string host, uint16_t port, uint16_t udp_port)
	: m_district(100, 100, getPhysicTerrainMap()),
      m_test_obj(&getPoolID(), true, shared_constants::k_player_object_type_id,
      	form1),
      m_test_obj2(&getPoolID(), true, shared_constants::k_player_object_type_id,
      	form2),
	  m_district_renderer(&m_district)
{
	setWorld(&m_district_renderer);
	ClientEngine::init(k_wnd_title, std::move(host), port, udp_port, k_wnd_width, k_wnd_height);
}

void GameEngine::onInit() {
	m_test_obj.moveTo(&m_district, 1.5, 1.5);
	m_test_obj.setMaxSpeed(8.0f);
	m_test_obj2.moveTo(&m_district, 4.5, 4.5);
	m_test_obj2.setMaxSpeed(5.27f);
	getRenderer()->getCurrentCamera()->setHeight(10.f);
	SpaceObjectGenerator::generateSpaceObjects(&m_district, 0.123f,
		shared_constants::k_wall_object_type_id, getPoolID());
	TerrainGenerator::generateTerrains(&m_district, 0.3f);
}

void GameEngine::initTerrains() {
}

void GameEngine::onLogicUpdate() {
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
	m_test_obj.setCurrentSpeed(v.x != 0 || v.y != 0 ? m_test_obj.getMaxSpeed(): 0.f);

	Vector v2 = getPlayer2KeyboardSpeedDirection();

	m_test_obj2.setSpeedDirection(v2);
	m_test_obj2.setCurrentSpeed(v2.x != 0 || v2.y != 0 ? m_test_obj2.getMaxSpeed() : 0.f);

	m_district.moveObjects(dt);
	dynamic_cast<Camera*>(getRenderer()->getCurrentCamera().get())->setOrigin(m_test_obj.getPosition());

	// m_session->sendPacket(flat_engine::network::SHARED_TEST_TEXT_MESSAGE,
	// 	flat_engine::network::Serializer::serializeTextMessage("123"));
	getSession()->sendReliable(flat_engine::network::SERVER_SEND_PLAYER_AVATAR,
		flat_engine::network::SharedSerializer::serializePlayerAvatar(
			m_test_obj.getPosition().m_coords.x, m_test_obj.getPosition().m_coords.y,
			m_test_obj.getPosition().m_index.x, m_test_obj.getPosition().m_index.y,
			v.x, v.y));
}

void GameEngine::initDisplayObjects() {
	constexpr uint32_t k_main_object_type_id = 0;

	auto texture_buffer = getRenderer()->getTextureManager();
	getObjectsTypesTextures().add(
		shared_constants::k_player_object_type_id,
		texture_buffer->loadTexture("assets/sprites/warrior.png"));

	getObjectsTypesTextures().add(
		shared_constants::k_wall_object_type_id,
		texture_buffer->loadTexture("assets/sprites/wall.jpg"));

	auto& display_terrain_map = getDisplayTerrainMap();
	display_terrain_map[0] = texture_buffer->loadTexture("assets/sprites/grass.png");
	display_terrain_map[1] = texture_buffer->loadTexture("assets/sprites/swamp.png");
	display_terrain_map[2] = texture_buffer->loadTexture("assets/sprites/sea.png");

	auto& physic_terrain_map = getPhysicTerrainMap();
	physic_terrain_map[0] = 2.f;
	physic_terrain_map[1] = 3.f;
	physic_terrain_map[2] = 4.f;
}

void GameEngine::onKeyDown(int key, int scancode, int action, int mods) {
	if (key == ScanCode::KEY_R)
		m_test_obj2.rotate(k_rotation_left);
}

void GameEngine::onRender() {

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
