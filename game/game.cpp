#include "game.hpp"
#include "chrono"
#include "utils/hash.hpp"
#include "physics/collisions.hpp"

Game::Game(const char* title, uint32_t width, uint32_t height)
	: Engine(title, width, height), m_district_net(10, 10, 10000, 20), 
	m_test_obj(true, ObjectForm(160)),
	m_test_obj2(true, ObjectForm(240, 240)),
	m_test_static_obj(false, ObjectForm(160, 160)), 
	m_test_static_obj2(false, ObjectForm(80)), 
	m_test_fly(true, ObjectForm(120)) 
{
}

void Game::onInit() {
	// —юда добавить код при инициализации
	District* district = m_district_net.addDistrict(0, 0);

	m_test_obj.moveTo(district, 900, 900);
	m_test_obj2.moveTo(district, 100, 100);
	m_test_static_obj.moveTo(district, 400, 400);
	m_test_static_obj2.moveTo(district, 600, 600);
	m_test_fly.moveTo(district, 200, 600);
}

void Game::onRender() {
	// —юда добавить код, выполн€ющийс€
	// перед обработкой кадра 
	static auto start_time = std::chrono::high_resolution_clock::now();

	auto current_time = std::chrono::high_resolution_clock::now();
	static auto undo_time = start_time;
	float dt = std::chrono::duration<float, std::chrono::seconds::period>
		(current_time - undo_time).count();
	undo_time = current_time;

	float ct = std::chrono::duration<float, std::chrono::seconds::period>
		(start_time - current_time).count();

	Vector v = getPlayerKeyboardSpeedDirection();

	m_test_obj.setSpeedDirection(v);
	m_test_obj.setCurrentSpeed(v.x != 0 || v.y != 0 ? 200: 0);

	Vector v2 = getPlayer2KeyboardSpeedDirection();

	m_test_obj2.setSpeedDirection(v2);
	m_test_obj2.setCurrentSpeed(v2.x != 0 || v2.y != 0 ? 200 : 0);

	Vector v3 = Vector(cos(ct), sin(ct), 0);

	m_test_fly.setSpeedDirection(v3);
	m_test_fly.setCurrentSpeed(100);

	//m_test_obj.move(dt);

	static glm::vec2 camera_shift{ 400, -400 };

	if (isKeyPressed(ScanCode::KEY_C)) {
		auto cam_pos = m_test_obj.getPosition();
		camera_shift.x = cam_pos.x;
		camera_shift.y = -cam_pos.y;
	}

	auto position = m_test_obj.getRenderOrigin();
	glm::vec2 mins = { -160, -160 };
	glm::vec2 maxs = -mins;

	g_renderer->drawRectTex( 
		mins + position - camera_shift, 
		maxs + position - camera_shift, 
		HASH( "circle_move" ) );

	auto position2 = m_test_obj2.getRenderOrigin();
	glm::vec2 mins2 = { -120, -120 };
	glm::vec2 maxs2 = -mins2;

	g_renderer->drawRectTex(
		mins2 + position2 - camera_shift, 
		maxs2 + position2 - camera_shift, 
		HASH("test"));

	auto position1 = m_test_static_obj.getRenderOrigin();
	glm::vec2 mins1 = { -80, -80 };
	glm::vec2 maxs1 = -mins1;

	g_renderer->drawRectTex(
		mins1 + position1 - camera_shift,
		maxs1 + position1 - camera_shift, 
		HASH("test2"));

	auto position3 = m_test_static_obj2.getRenderOrigin();
	glm::vec2 mins3 = { -80, -80 };
	glm::vec2 maxs3 = -mins3;

	g_renderer->drawRectTex(
		mins3 + position3 - camera_shift, 
		maxs3 + position3 - camera_shift, 
		HASH("circle_static"));

	auto position4 = m_test_fly.getRenderOrigin();
	glm::vec2 mins4 = { -120, -120 };
	glm::vec2 maxs4 = -mins4;

	g_renderer->drawRectTex(
		mins4 + position4 - camera_shift, 
		maxs4 + position4 - camera_shift, 
		HASH("circle_move"));

	static auto output_time = start_time;
	if ( ( std::chrono::duration<float, std::chrono::seconds::period>
		( current_time - output_time ).count( ) ) > 1 ) {
		output_time = current_time;
		// input debug here (every second).
		printf( "%f %f\n", 
			m_test_obj.getPosition().x, 
			m_test_obj.getPosition().y);
	}

	m_district_net.moveObjects(dt);
}

Vector Game::getPlayerKeyboardSpeedDirection() {
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

Vector Game::getPlayer2KeyboardSpeedDirection() {
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