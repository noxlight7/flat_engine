#include "client_engine.hpp"
#include <iostream>
#include <utility>

void ClientEngine::initTime() {
	m_start_frame_processing_time = std::chrono::steady_clock::now();
	m_delta_time = 0;
}

void ClientEngine::drawInterface() {

}

void ClientEngine::initThreads() {
	m_async_thread = std::thread([this] {
		m_io_context.run();
	});
}

void ClientEngine::updateTime() {
	auto current_time = std::chrono::steady_clock::now();
	m_delta_time = std::chrono::duration<float, std::chrono::seconds::period>
		(current_time - m_start_frame_processing_time).count();
	m_start_frame_processing_time = current_time;
	// std::cout << "Time elapsed: " << m_delta_time << std::endl;
}

ClientEngine::ClientEngine(boost::asio::io_context& io_context, std::string host, uint16_t port)
	: m_width(0), m_height(0),
	  m_session(std::make_shared<flat_engine::network::ClientSession>(
	  	io_context, std::move(host), port)), m_world(nullptr),
	  m_window(), m_delta_time(0), m_io_context(io_context), m_start_frame_processing_time() {
}

void ClientEngine::init(const std::string& title, uint32_t width, uint32_t height) {
	m_width = width;
	m_height = height;
	initThreads();
    m_session->start();
    // m_session->sendPacket(flat_engine::network::SHARED_TEST_TEXT_MESSAGE,
    // 	flat_engine::network::Serializer::serializeTextMessage("Игра подключена успешно"));
	initWindow(title);
	memset(m_button_state, 0, sizeof(bool) * ScanCode::KEYS_AMOUNT);
	initTime();
}

void ClientEngine::initWindow(const std::string& title) {
	glfwInit();
	m_window = glfwCreateWindow(m_width, m_height, title.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent( m_window );
	glfwSwapInterval( m_vsync ); // turns glfw vsync.

	g_renderer->init( m_window, getAllTexturesFullNames() );
	g_renderer->resizeFrameBuffer( m_window, m_width, m_height );

	glfwSetWindowUserPointer(m_window, this);
	setCallbacks();

}

void ClientEngine::setCallbacks() {
	glfwSetFramebufferSizeCallback(m_window, windowResizeCallback);
	glfwSetKeyCallback(m_window, keyCallback);
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

void ClientEngine::windowResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<ClientEngine*>(glfwGetWindowUserPointer(window));
	//app->renderer->resizeFrameBuffer();
	app->m_width = width;
	app->m_height = height;

	app->onResize(width, height);
	g_renderer->resizeFrameBuffer( window, width, height );
}

void ClientEngine::mainLoop() {
	while (!glfwWindowShouldClose(m_window)) {
		updateTime();
		// clear buffers.
		glClearColor( 0.2, 0.0, 0.0, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		onRender();

		g_renderer->prepareFrame();

		if (m_world)
			m_world->drawWorld();

		g_renderer->drawFrame();

		glfwSwapBuffers( m_window );
		glfwPollEvents( );
	}
}

void ClientEngine::run() {
	onInit();

	initTime();

	mainLoop();

	m_session->stop();
	// m_io_context.stop();
	m_async_thread.join();
}

bool ClientEngine::isKeyPressed(int scancode) {
	return m_button_state[scancode];
}

// bool ClientEngine::connect(const std::string& host, unsigned short port) {
//     try {
//         tcp::resolver resolver(io_context_);
//         auto endpoints = resolver.resolve(host, std::to_string(port));
//
//         boost::asio::connect(socket_, endpoints);
//         is_connected_ = true;
//
//         // Запускаем сетевой поток
//         network_thread_ = std::thread([this]() {
//             startReceive();
//             io_context_.run();
//         });
//
//         return true;
//     }
//     catch (std::exception& e) {
//         std::cerr << "Connection error: " << e.what() << std::endl;
//         return false;
//     }
// }
//
// void ClientEngine::send(const std::vector<uint8_t>& data) {
//     if (!is_connected_) return;
//
//     boost::asio::async_write(socket_, boost::asio::buffer(data),
//         [this](boost::system::error_code ec, std::size_t /*length*/) {
//             if (ec) {
//                 is_connected_ = false;
//                 onDisconnected();
//             }
//         });
// }
//
// void ClientEngine::startReceive() {
//     if (!is_connected_) return;
//
//     socket_.async_read_some(boost::asio::buffer(receive_buffer_),
//         [this](boost::system::error_code ec, std::size_t length) {
//             if (!ec) {
//                 onDataReceived(receive_buffer_, length);
//                 startReceive(); // Продолжаем чтение
//             }
//             else {
//                 is_connected_ = false;
//                 onDisconnected();
//             }
//         });
// }