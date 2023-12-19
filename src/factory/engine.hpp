#pragma once

#include "../entry.hpp"

#include "../input/input.hpp"

class Engine {
public:
	//void StartUp( );

	Engine(const char* title, uint32_t width = 1024, uint32_t height = 768);
	~Engine();
	void run();

	// ����������� ����� �������� ����� ��������� ���������
	virtual void onInit() {}
	virtual void onDraw() {}
	virtual void onClose() {}
	// ����������� ��� ������ ����������
	virtual void onRender() {}
	virtual void onDestroy() {}
	virtual void onMouseMove() {}
	virtual void onMouseDown() {}
	virtual void onMouseUp() {}
	// ����������� ��� ������� �������
	virtual void onKeyDown(int key, int scancode, int action, int mods) {}
	// ����������� ��� ������� �������
	virtual void onKeyUp(int key, int scancode, int action, int mods) {}
	// ����������� ��� ��������� ������� ����
	virtual void onResize(int width, int height) {}
	// ��������
	// Renderer* m_renderer;

	// ���������� true, ���� ������� scancode ������
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