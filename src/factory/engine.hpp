#pragma once

#include "../entry.hpp"

#include "../input/input.hpp"

class Engine {
public:
	//void StartUp( );

	Engine(const char* title, uint32_t width = 1024, uint32_t height = 768);
	~Engine();
	void run();

	// Выполняется перед запуском цикла обработки сообщений
	virtual void onInit() {}
	virtual void onDraw() {}
	virtual void onClose() {}
	// Выполняется при каждой прорисовке
	virtual void onRender() {}
	virtual void onDestroy() {}
	virtual void onMouseMove() {}
	virtual void onMouseDown() {}
	virtual void onMouseUp() {}
	// Выполняется при нажатии клавиши
	virtual void onKeyDown(int key, int scancode, int action, int mods) {}
	// Выполняется при отжатии клавиши
	virtual void onKeyUp(int key, int scancode, int action, int mods) {}
	// Выполняется при изменении размера окна
	virtual void onResize(int width, int height) {}
	// Рендерер
	// Renderer* m_renderer;

	// Возвращает true, если клавиша scancode нажата
	bool isKeyPressed(int scancode);

protected:
	uint32_t m_width, m_height;
	bool m_pressed_buttons[ScanCode::KEYS_AMOUNT];

private:
	GLFWwindow* m_window;

	void initWindow(const char* title);
	void mainLoop();
	void setCallbacks();
	static void windowResizeCallback(GLFWwindow* window, int width, int height);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};