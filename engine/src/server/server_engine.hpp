#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <iostream>
#include <network_server.h>
#include <utils/scheduler.hpp>

#include "deserializer.h"

class ServerEngine {
    boost::asio::io_context m_io_context;
    // Scheduler m_scheduler;
    flat_engine::network::NetworkServer m_network_server;

    float m_delta_time = 0;
    std::chrono::steady_clock::time_point m_last_update;

    std::vector<std::thread> m_threads;

    void initThreads() {
        unsigned int num_threads = std::thread::hardware_concurrency() / 2;
        // unsigned int num_threads = 1;
        if (num_threads < 1)
            num_threads = 1;

        for (unsigned int i = 0; i < num_threads; i++) {
            m_threads.emplace_back([this, i] {
                const unsigned int thread_index = i + 1;
                m_io_context.run();
                std::cout << "[Server] thread " + std::to_string(thread_index) +
                    "completed\n";
            });
        }
    }
public:
    explicit ServerEngine(uint16_t tcp_port, uint16_t udp_port,
        std::unique_ptr<flat_engine::network::ISessionController> session_controller)
        :
    // m_scheduler(m_io_context),
    m_network_server(m_io_context, tcp_port, udp_port, std::move(session_controller)) {
    }

    virtual ~ServerEngine() = default;

    void init() {
        initThreads();
        run();
    }

    void run() {
        {
            constexpr double ticks_per_second = 30.0;
            const auto tick_duration_ms = std::chrono::milliseconds(static_cast<uint32_t>(1000. / ticks_per_second));
            using clock = std::chrono::steady_clock;
            std::atomic running(true);
            while (running.load()) {
                auto tick_start = clock::now();
                auto tick_next = tick_start + tick_duration_ms;
                // processGameLogic();
                flat_engine::network::Deserializer::instance().processAllMessages(m_threads.size());
                auto tick_end = clock::now();
                if (tick_end - tick_start < tick_duration_ms)
                    std::this_thread::sleep_for(tick_next - tick_end);
            }
        }

        for (auto &t : m_threads) {
            t.join();
        }
    }
};
