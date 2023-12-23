#include "game.hpp"
#include "chrono"
#include "utils/hash.hpp"
#include "physics/collisions.hpp"

Game::Game(const char* title, uint32_t width, uint32_t height)
	: Engine(title, width, height), m_district_net(10, 10, 1000, 100), m_test_obj(ObjectForm(100)),
	m_test_static_obj(ObjectForm(100)) {

}

void Game::onInit() {
	// —юда добавить код при инициализации
	District* district = m_district_net.addDistrict(0, 0);

	MoveableObject* mo = new MoveableObject();
	mo->moveTo(district, 50, 50);
	delete mo;

	m_test_obj.moveTo(district, 500, 500);
	m_test_static_obj.moveTo(district, 100, 100);
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

	Vector v = getPlayerKeyboardSpeedDirection();

	m_test_obj.setSpeedDirection(v);
	m_test_obj.setCurrentSpeed(v.x != 0 || v.y != 0 ? 500: 0);

	m_test_obj.move(dt);

	auto position = m_test_obj.getRenderOrigin();
	glm::vec2 mins = { -160, -160 };
	glm::vec2 maxs = -mins;

	g_vertex_buffer->addRect( mins + position, maxs + position, { 1.0,1.0,1.0,1.0 }, HASH( "test" ) );

	auto position1 = m_test_static_obj.getRenderOrigin();
	glm::vec2 mins1 = { -160, -160 };
	glm::vec2 maxs1 = -mins1;

	g_vertex_buffer->addRect(mins1 + position1, maxs1 + position1, { 1.0,1.0,1.0,1.0 }, HASH("test2"));

	double dt2 = glfwGetTime( );
	glfwSetTime( 0 );
	static auto output_time = start_time;
	if ( ( std::chrono::duration<float, std::chrono::seconds::period>
		( current_time - output_time ).count( ) ) > 1 ) {
		output_time = current_time;
		// input debug here (every second).
		printf( "%f %lf\n", dt, dt2 );
	}

	//float colTime;
	//if (!Collisions::Collision(&m_test_obj, &m_test_static_obj, dt, &colTime))
		//m_test_obj.move(dt);
	//else
		//m_test_obj.move(colTime);
}

Vector&& Game::getPlayerKeyboardSpeedDirection() {
	Vector v_direct{};

	if (isKeyPressed(ScanCode::KEY_W))
		v_direct.y = 1;
	if (isKeyPressed(ScanCode::KEY_S))
		v_direct.y = -1;
	if (isKeyPressed(ScanCode::KEY_D))
		v_direct.x = 1;
	if (isKeyPressed(ScanCode::KEY_A))
		v_direct.x = -1;

	return std::move(v_direct);
}