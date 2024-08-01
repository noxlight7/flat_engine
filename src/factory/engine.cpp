#include "engine.hpp"
//void Engine::StartUp( ) {
//	printf( "hello" );
//}

void Engine::initTime() {
	m_start_frame_processing_time = std::chrono::steady_clock::now();
	m_delta_time = 0;
}

void Engine::drawInterface() {

}

void Engine::updateTime() {
	auto current_time = std::chrono::steady_clock::now();
	m_delta_time = std::chrono::duration<float, std::chrono::seconds::period>
		(current_time - m_start_frame_processing_time).count();
	m_start_frame_processing_time = current_time;
}

Engine::Engine() : m_world(nullptr), m_height(0), m_width(0), m_delta_time(0),
m_start_frame_processing_time(), m_window() {
}

void Engine::init(const char* title, uint32_t width, uint32_t height) {
	m_width = width;
	m_height = height;
	initWindow(title);
	memset(m_button_state, 0, sizeof(bool) * ScanCode::KEYS_AMOUNT);
	initTime();
}

void Engine::initWindow(const char* title) {
	glfwInit();

	m_window = glfwCreateWindow(m_width, m_height, title, nullptr, nullptr);
	glfwMakeContextCurrent( m_window );
	glfwSwapInterval( m_vsync ); // turns glfw vsync.

	g_renderer->init( m_window );
	g_renderer->resizeFrameBuffer( m_window, m_width, m_height );

	glfwSetWindowUserPointer(m_window, this);
	setCallbacks();

}

void Engine::setCallbacks() {
	glfwSetFramebufferSizeCallback(m_window, windowResizeCallback);
	glfwSetKeyCallback(m_window, keyCallback);
}

Engine::~Engine() {
	//delete renderer;
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Engine::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto app = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
	switch (action)
	{
	case GLFW_PRESS:
		app->m_button_state[key] = true;
		app->onKeyDown(key, scancode, action, mods);
		break;
	case GLFW_RELEASE:
		app->m_button_state[key] = false;
		app->onKeyUp(key, scancode, action, mods);
		break;
	}

}

void Engine::windowResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
	//app->renderer->resizeFrameBuffer();
	app->m_width = width;
	app->m_height = height;

	app->onResize(width, height);
	g_renderer->resizeFrameBuffer( window, width, height );
}

void Engine::mainLoop() {
	while (!glfwWindowShouldClose(m_window)) {
		// clear buffers.
		glClearColor( 0.2, 0.0, 0.0, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		onRender();

		g_renderer->prepareFrame();

		if (m_world)
			m_world->drawWorld();

		g_renderer->drawFrame();

		/*static bool bx = false;
		static bool by = false;
		if ( isKeyPressed( ScanCode::KEY_W ) && !bx ) {
			g_renderer->g_test_entity.m_render_origin.x += 1.0f;
			bx = true;
		}if ( isKeyPressed( ScanCode::KEY_S ) && !by ) {
			g_renderer->g_test_entity.m_render_origin.x -= 1.0f;
			by = true;
		}*/

		glfwSwapBuffers( m_window );
		glfwPollEvents( );
	}
}

void Engine::run() {
	onInit();

	initTime();

	mainLoop();
}

bool Engine::isKeyPressed(int scancode) {
	return m_button_state[scancode];
}