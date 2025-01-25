//
// Created by curo on 01.01.2025.
//


#include <iostream>
#include "network_server.hpp"

#include "deserializer.h"

namespace flat_engine::network {
    uint32_t NetworkServer::nextSessionId() {
        while (sessions_.contains(++current_session_id_))
            ++current_session_id_;
        return current_session_id_;
    }

    void NetworkServer::startAccept() {
        if (!acceptor_.is_open()) {
            return;
        }
        auto socket = make_shared<boost::asio::ip::tcp::socket>(io_context_);
        acceptor_.async_accept(*socket,
            [this, socket](const boost::system::error_code& ec) {
                if (!ec) {
                    boost::asio::dispatch(strand_, [socket, this]() {
                        std::cout << "[Server] Подключён новый клиент" << std::endl;
                        uint32_t new_session_id = nextSessionId();
                        sessions_[new_session_id] = std::make_shared<ServerSession>(io_context_,
                            std::move(*socket), [this](uint32_t session_id) {
                                removeSession(session_id);
                            }, new_session_id);
                        sessions_[new_session_id]->start();
                    });

                    boost::asio::post(strand_, [this] {
                        startAccept();
                    });

                } else {
                    if (ec != boost::system::errc::operation_canceled)
                        std::cerr << "[Server] Ошибка accept " << ec.message() << std::endl;
                }
            });
    }

    NetworkServer::NetworkServer(boost::asio::io_context& io_context, const uint16_t port)
            : io_context_(io_context), strand_(make_strand(io_context)),
              acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
              port_(port), current_session_id_(0) {
        startAccept();
    }

    void NetworkServer::removeSession(const uint32_t session_id) {
        post(strand_, [this, session_id] {
            if (!sessions_.contains(session_id))
                return;

            sessions_.erase(session_id);
            std::cout << "[Server] Сессия с id=" << session_id << " удалена" << std::endl;
        });
    }

    void processGameLogic() {
        // Допустим, здесь идёт обработка игровой логики
    }
}
//
//using namespace flat_engine::network;
//
//int main(int argc, char **argv) {
//    GameDeserializeRoutes::initRoutes();
//
//    boost::asio::io_context io_context;
//    unsigned int num_threads = std::thread::hardware_concurrency() / 2;
//    if (num_threads == 0)
//        num_threads = 2;
//
//    auto server = NetworkServer(io_context, 5000);
//    std::vector<std::thread> threads;
//    for (unsigned int i = 0; i < num_threads; i++) {
//        threads.emplace_back([&io_context, i] {
//            const unsigned int thread_index = i + 1;
//            io_context.run();
//            std::cout << "[Server] thread " + std::to_string(thread_index) +
//                "completed\n";
//        });
//    }
//
//    {
//        constexpr double ticks_per_second = 30.0;
//        const auto tick_duration_ms = std::chrono::milliseconds(static_cast<uint32_t>(1000. / ticks_per_second));
//        using clock = std::chrono::steady_clock;
//        std::atomic running(true);
//        while (running.load()) {
//            auto tick_start = clock::now();
//            auto tick_next = tick_start + tick_duration_ms;
//            processGameLogic();
//            Deserializer::instance().processAllMessages(num_threads);
//            auto tick_end = clock::now();
//            if (tick_end - tick_start < tick_duration_ms)
//                std::this_thread::sleep_for(tick_next - tick_end);
//        }
//    }
//
//    for (auto &t : threads) {
//        t.join();
//    }
//
//    return 0;
//}