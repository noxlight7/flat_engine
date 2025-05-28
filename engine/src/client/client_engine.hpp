#pragma once

#include "../entry.hpp"
#include "../input/input.hpp"
#include "../display/renderer_world.hpp"
#include "client/network_client.h"

#include <boost/asio.hpp>
#include <thread>
#include <array>
#include <physics/physics.hpp>

#include <renderer.hpp>
#include <utils/scheduler.hpp>

#include "display/display_objects.hpp"
#include "display/display_system.hpp"
#include "utils/pool_id.hpp"

using tcp = boost::asio::ip::tcp;

class ClientEngine {
    boost::asio::io_context m_receiving_context;
    boost::asio::io_context m_processing_context;
    Scheduler m_scheduler;
    PoolID m_pool_id;
    thread m_message_receiving_thread;
    thread m_processing_thread;
    uint32_t m_width, m_height;
    bool m_button_state[KEYS_AMOUNT]{};
    bool m_vsync{ true };
    std::shared_ptr<flat_engine::network::ClientSession> m_session;
    DisplaySystem m_display_system;
    DisplayObjects m_objects_types_textures;
    std::unique_ptr<IRenderer> m_renderer{};
    PhysicTerrainMap m_physic_terrain_map;
    DisplayTerrainMap m_display_terrain_map;

    std::mutex m_renderer_mutex;

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
    static void windowCloseCallback(GLFWwindow* window);

protected:
    virtual ~ClientEngine();
    ClientEngine();

public:
    void init(const std::string& title, std::string host, uint16_t port, uint16_t udp_port,
        uint32_t width = 1024, uint32_t height = 768);
    virtual void run();

    // Выполняется перед запуском цикла обработки сообщений
    virtual void onInit() {}
    virtual void onDraw() {}
    virtual void onClose() {}
    // Выполняется при каждой прорисовке
    virtual void onRender() {}
    virtual void onLogicUpdate() {}
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

    IRendererWorld* getWorld() { return m_world; }
    void setWorld(IRendererWorld* world) { m_world = world; }

    float getDeltaTime() const { return m_delta_time; }
    std::chrono::steady_clock::time_point
        getStartFrameProcessingTime() const { return m_start_frame_processing_time; }
    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

    virtual void initDisplayObjects() = 0;

    DisplayTerrainMap& getDisplayTerrainMap() { return m_display_terrain_map; }
    PhysicTerrainMap& getPhysicTerrainMap() { return m_physic_terrain_map; }

    IRenderer* getRenderer() const { return m_renderer.get(); }
    PoolID& getPoolID() { return m_pool_id; }
    std::shared_ptr<flat_engine::network::ClientSession>& getSession() { return m_session; }
    DisplayObjects& getObjectsTypesTextures() { return m_objects_types_textures; }

};