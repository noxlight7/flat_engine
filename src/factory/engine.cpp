#include "engine.hpp"

//void Engine::StartUp( ) {
//	printf( "hello" );
//}

Engine::Engine(const char* title, uint32_t width, uint32_t height)
	: m_width(width), m_height(height)
{
	initWindow(title);
	memset(m_pressed_buttons, 0, sizeof(bool) * ScanCode::KEYS_AMOUNT);
}

void Engine::initWindow(const char* title) {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_window = glfwCreateWindow(m_width, m_height, title, nullptr, nullptr);
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
		app->m_pressed_buttons[key] = true;
		app->onKeyDown(key, scancode, action, mods);
		break;
	case GLFW_RELEASE:
		app->m_pressed_buttons[key] = false;
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
}

void Engine::mainLoop() {
	while (!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();
		onRender();
		//renderer->drawFrame();
	}
}

void Engine::run() {
	onInit();

	mainLoop();
}

bool Engine::isKeyPressed(int scancode) {
	return m_pressed_buttons[scancode];
}