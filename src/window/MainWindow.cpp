//#include <glfw/src/glfw3.h>
#include <memory.h>
#include "MainWindow.hpp"

MainWindow::MainWindow(const char* title, uint32_t width, uint32_t height)
	: m_width(width), m_height(height)
{
	initWindow(title);
	memset(pressedButtons, 0, sizeof(bool) * ScanCode::KEY_LAST);
}

void MainWindow::initWindow(const char* title) {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_window = glfwCreateWindow(m_width, m_height, title, nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);
	setCallbacks();
}

void MainWindow::setCallbacks()
{
	glfwSetFramebufferSizeCallback(m_window, windowResizeCallback);
	glfwSetKeyCallback(m_window, keyCallback);
}

MainWindow::~MainWindow()
{
	//delete renderer;
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void MainWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto app = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
	switch (action)
	{
	case GLFW_PRESS: 
		app->pressedButtons[key] = true;
		app->onKeyDown(key, scancode, action, mods);
		break;
	case GLFW_RELEASE: 
		app->pressedButtons[key] = false;
		app->onKeyUp(key, scancode, action, mods);
		break;
	}
		
}

void MainWindow::windowResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
	//app->renderer->resizeFrameBuffer();
	app->m_width = width;
	app->m_height = height;

	app->onResize(width, height);
}

void MainWindow::mainLoop() {
	while (!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();
		onDraw();
		//renderer->drawFrame();
	}
}

void MainWindow::run()
{
	onInit();

	mainLoop();
}