#include "client_engine.hpp"
#include <iostream>
#include <utility>

#include "display/game_camera.hpp"

void ClientEngine::initTime() {
	m_start_frame_processing_time = std::chrono::steady_clock::now();
	m_delta_time = 0;
}

void ClientEngine::drawInterface() {

}

void ClientEngine::initThreads() {
	m_message_receiving_thread = std::thread([this] {
		std::cout << "Receiving thread run" << std::endl;
		m_receiving_context.run();
	});

	m_processing_thread = std::thread([this] {
		m_scheduler.schedulePeriodic(k_logic_update_interval, [this] {
			onLogicUpdate();
			updateTime();
			if (m_world)
				m_world->drawWorld(m_display_system, m_objects_types_textures, m_renderer.get());
		});
		m_processing_context.run();
	});
}

void ClientEngine::updateTime() {
	auto current_time = std::chrono::steady_clock::now();
	m_delta_time = std::chrono::duration<float, std::chrono::seconds::period>
		(current_time - m_start_frame_processing_time).count();
	m_start_frame_processing_time = current_time;
}

ClientEngine::ClientEngine()
	: m_scheduler(m_processing_context),
	  m_pool_id(k_cold_pool_amount, k_pool_shift_period, m_scheduler),
	  m_width(0), m_height(0), m_display_system(m_renderer_mutex),
	  m_world(nullptr), m_window(), m_delta_time(0), m_start_frame_processing_time() {
}

void ClientEngine::init(const std::string& title, std::string host, uint16_t port, uint32_t width, uint32_t height) {
	m_width = width;
	m_height = height;
	m_session = std::make_shared<flat_engine::network::ClientSession>(
		m_receiving_context, std::move(host), port);
	m_session->start();
	// m_session->sendPacket(flat_engine::network::SHARED_TEST_TEXT_MESSAGE,
    // 	flat_engine::network::Serializer::serializeTextMessage("Игра подключена успешно"));
	initWindow(title);
	memset(m_button_state, 0, sizeof(bool) * ScanCode::KEYS_AMOUNT);
	initTime();
	m_pool_id.init();

	onInit();

	initThreads();
}

void ClientEngine::initWindow(const std::string& title) {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(m_width, m_height, title.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent( m_window );
	glfwSwapInterval( m_vsync ); // turns glfw vsync.

	m_renderer = IRenderer::createRenderer(RendererType::OPEN_GL);
	if (!m_renderer->init(m_window)) {
		glfwTerminate();
		exit(-1);
	}
	m_renderer->init(m_window);
	m_renderer->setCurrentCamera(std::make_shared<Camera>(glm::vec3(0.0, 0.0, 5.0)));
	m_renderer->updateViewPortSize();
	m_display_system.init(m_renderer.get());

	initDisplayObjects();

	glfwSetWindowUserPointer(m_window, this);
	setCallbacks();

}

void ClientEngine::setCallbacks() {
	glfwSetFramebufferSizeCallback(m_window, windowResizeCallback);
	glfwSetKeyCallback(m_window, keyCallback);
	glfwSetWindowCloseCallback(m_window, windowCloseCallback);
}

ClientEngine::~ClientEngine() {
	//delete renderer;
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void ClientEngine::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto app = reinterpret_cast<ClientEngine*>(glfwGetWindowUserPointer(window));
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

void ClientEngine::windowCloseCallback(GLFWwindow* window) {
	auto app = reinterpret_cast<ClientEngine*>(glfwGetWindowUserPointer(window));
	app->onClose();
	app->m_scheduler.cancelAllEvents();
}

void ClientEngine::windowResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<ClientEngine*>(glfwGetWindowUserPointer(window));
	//app->renderer->resizeFrameBuffer();
	app->m_width = width;
	app->m_height = height;

	app->onResize(width, height);
	app->m_renderer->updateViewPortSize();
}

void ClientEngine::mainLoop() {
	while (!glfwWindowShouldClose(m_window)) {
		// clear buffers.


		onRender();

		m_renderer->updateViewPortSize();
		m_renderer->beginFrame();

		m_renderer->clear( 0.1, 0.1, 0.2, 1.0 );
		// glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		auto current_time = std::chrono::steady_clock::now();
		float logic_interval_seconds = std::chrono::duration<float>(k_logic_update_interval).count();
		float delta_since_logic_update = std::chrono::duration<float>(
			current_time - m_start_frame_processing_time).count();
		float interpolation_alpha = 1 - delta_since_logic_update / logic_interval_seconds;
		if (m_world)
			m_display_system.draw(interpolation_alpha, m_renderer->getCurrentCamera()->getHeight());
		else
			m_display_system.draw(interpolation_alpha, 0);

		std::cout << "interpolation alpha: " << interpolation_alpha << std::endl;

		m_renderer->endFrame();

		// glfwSwapBuffers( m_window );
		glfwPollEvents( );
	}
}

void ClientEngine::run() {
	initTime();

	mainLoop();

	m_session->stop();
	// m_io_context.stop();
	m_message_receiving_thread.join();
	m_processing_thread.join();
}

bool ClientEngine::isKeyPressed(int scancode) {
	return m_button_state[scancode];
}
