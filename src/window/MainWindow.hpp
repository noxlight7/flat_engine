#pragma once

#include <glfw/src/glfw3.h>
#include "IOKeys.h"

// Класс окна
// Предполагается, что игра создаст собственный класс,
// наследуя его от этого
class MainWindow
{
	//friend Renderer;
public:
	MainWindow(const char* title, uint32_t width=1024, uint32_t height=768);
	virtual ~MainWindow();
	void run();

	virtual void onInit() {}
	virtual void onDraw() {}
	virtual void onClose() {}
	virtual void onRender() {}
	virtual void onDestroy() {}
	virtual void onMouseMove() {}
	virtual void onMouseDown() {}
	virtual void onMouseUp() {}
	virtual void onKeyDown(int key, int scancode, int action, int mods) {}
	virtual void onKeyUp(int key, int scancode, int action, int mods) {}
	virtual void onResize(int width, int height) {}
	// Рендерер
	// Renderer* renderer;

protected:
	uint32_t m_width, m_height;
	bool pressedButtons[ScanCode::KEY_LAST];
private:

	GLFWwindow* m_window;

	void initWindow(const char* title);
	void mainLoop();
	void setCallbacks();
	static void windowResizeCallback(GLFWwindow* window, int width, int height);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};