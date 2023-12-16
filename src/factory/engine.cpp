#include "engine.hpp"

void Engine::StartUp( ) {
	printf( "hello" );
	wnd = new MainWindow("flat_game");
	wnd->run();
}