#pragma once

#include "../entry.hpp"
#include "../input/input.hpp"
#include "../display/renderer.hpp"
#include "client/network_client.h"

#include <boost/asio.hpp>
#include <thread>
#include <array>
#include <vector>

using tcp = boost::asio::ip::tcp;

class ClientEngine {
public:
    void init(const std::string& title, uint32_t width = 1024, uint32_t height = 768);
    virtual void run();

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

    // Возвращает true, если клавиша scancode нажата
    bool isKeyPressed(int scancode);

    inline IRendererWorld* getWorld() { return m_world; }
    inline void setWorld(IRendererWorld* world) { m_world = world; }

    inline float getDeltaTime() { return m_delta_time; }
    inline std::chrono::steady_clock::time_point 
        getStartFrameProcessingTime() { return m_start_frame_processing_time; }
    inline uint32_t getWidth() { return m_width; }
    inline uint32_t getHeight() { return m_height; }

    // Сетевые методы
    // bool connect(const std::string& host, unsigned short port);
    // void send(const std::vector<uint8_t>& data);
    // virtual void onDataReceived(const std::array<uint8_t, 1024>& data, std::size_t length) {}
    // virtual void onDisconnected() {}
    virtual std::vector<std::string> getAllTexturesFullNames() { return {}; }

protected:
    boost::asio::io_context& m_io_context;
    thread m_async_thread;
    uint32_t m_width, m_height;
    bool m_button_state[KEYS_AMOUNT]{};
    bool m_vsync{ true };
    std::shared_ptr<flat_engine::network::ClientSession> m_session;

    virtual ~ClientEngine();
    ClientEngine(boost::asio::io_context& io_context, std::string host, uint16_t port);


private:
    IRendererWorld* m_world;
    GLFWwindow* m_window;
    float m_delta_time;
    std::chrono::steady_clock::time_point m_start_frame_processing_time;

    void drawInterface();
    void initThreads();
    void initTime();
    void updateTime();
    void initWindow(const std::string& title);
    void mainLoop();
    void setCallbacks();
    static void windowResizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    // void startReceive();
};