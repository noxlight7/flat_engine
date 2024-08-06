#pragma once

#include "../entry.hpp"

#include "../input/input.hpp"

#include "display/renderer.hpp"

class Engine {
public:
	//void StartUp( );

	Engine();
	void init(const char* title, uint32_t width = 1024, uint32_t height = 768);
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

	inline IRendererWorld* getWorld() { return m_world; }
	inline void setWorld(IRendererWorld* world) { m_world = world; }

	inline float getDeltaTime() { return m_delta_time; }
	inline std::chrono::steady_clock::time_point 
		getStartFrameProcessingTime() { return m_start_frame_processing_time; }
	inline uint32_t getWidth() { return m_width; }
	inline uint32_t getHeight() { return m_height; }
protected:
	uint32_t	m_width, m_height;
	bool		m_button_state[KEYS_AMOUNT];
	bool		m_vsync{ true };
private:
	IRendererWorld* m_world;
	GLFWwindow*	m_window;
	float		m_delta_time;
	std::chrono::steady_clock::time_point m_start_frame_processing_time;

	void drawInterface();
	void initTime();
	void updateTime();
	void initWindow(const char* title);
	void mainLoop();
	void setCallbacks();
	static void windowResizeCallback(GLFWwindow* window, int width, int height);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};