#pragma once

//#include "../entry.hpp"
#include <glfw/src/glfw3.h>
#include "../IO/IOKeys.h"

class Engine {
public:
	//void StartUp( );

	Engine(const char* title, uint32_t width = 1024, uint32_t height = 768);
	~Engine();
	void run();

	// Выполняется перед запуском цикла обработки сообщений
	void onInit() {}
	void onDraw() {}
	void onClose() {}
	// Выполняется при каждой прорисовке
	void onRender() {}
	void onDestroy() {}
	void onMouseMove() {}
	void onMouseDown() {}
	void onMouseUp() {}
	// Выполняется при нажатии клавиши
	void onKeyDown(int key, int scancode, int action, int mods) {}
	// Выполняется при отжатии клавиши
	void onKeyUp(int key, int scancode, int action, int mods) {}
	// Выполняется при изменении размера окна
	void onResize(int width, int height) {}
	// Рендерер
	// Renderer* renderer;

protected:
	uint32_t m_width, m_height;
	bool pressedButtons[ScanCode::KEYS_AMOUNT];

private:
	GLFWwindow* m_window;

	void initWindow(const char* title);
	void mainLoop();
	void setCallbacks();
	static void windowResizeCallback(GLFWwindow* window, int width, int height);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};