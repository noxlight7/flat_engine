#include "game.hpp"
#include "chrono"

Game::Game(const char* title, uint32_t width, uint32_t height)
	: Engine(title, width, height), m_district_net(10, 10, 1000, 100), m_test_obj(nullptr) {

}

void Game::onInit() {
	// —юда добавить код при инициализации
	
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
	m_test_obj.setCurrentSpeed(v.x != 0 || v.y != 0 ? 10: 0);

	m_test_obj.move(dt);

	auto position = m_test_obj.getPosition();

	static auto m_output_time = start_time;
	if ((std::chrono::duration<float, std::chrono::seconds::period>
		(current_time - m_output_time).count()) > 1) {
		m_output_time = current_time;

		printf("Current position = (%f;%f), dt = %f\n", position.x, position.y, dt);
	}
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