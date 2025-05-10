#include "client_game_engine/game.hpp"

int main( ) {
#ifdef _WIN32
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
#endif // _WIN32

	GameEngine::instance().run();
}
