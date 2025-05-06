//
// Created by curo on 01.01.2025.
//


#include <iostream>
#include "network_server.h"

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
                    boost::asio::dispatch(strand_, [socket, this] {
                        std::cout << "[Server] Подключён новый клиент" << std::endl;
                        uint32_t new_session_id = nextSessionId();
                        sessions_[new_session_id] = std::make_shared<ServerSession>(io_context_,
                            std::move(*socket), [this](uint32_t session_id) {
                                removeSession(session_id);
                            }, new_session_id, udp_port_, udp_socket_, session_controller_->generateStartData());
                        sessions_[new_session_id]->start();

                        if (session_controller_)
                            session_controller_->onConnect(new_session_id);
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

    void NetworkServer::initCoreRoutes() {
        Deserializer::instance().registerHandler(
        CORE_CONNECT_UDP,
        [this](std::vector<uint8_t>&& buf, std::shared_ptr<ISession> session) -> IMessage* {
            uint16_t net_port;
            std::memcpy(&net_port, buf.data(), sizeof(net_port));
            uint16_t client_port = ntohs(net_port);
            const auto server_session = std::static_pointer_cast<ServerSession>(session);
            const auto address = server_session->getAddress();
            boost::asio::ip::udp::endpoint client_endpoint(address, client_port);
            server_session->bindUdpEndpoint(std::move(client_endpoint), *this->dispatcher_);
            return nullptr;
        });
    }

    NetworkServer::NetworkServer(
        boost::asio::io_context& io_context,
        const uint16_t tcp_port,
        const uint16_t udp_port,
        std::unique_ptr<ISessionController> session_controller)
            : io_context_(io_context), strand_(make_strand(io_context)),
              acceptor_(io_context, boost::asio::ip::tcp::endpoint(
                  boost::asio::ip::tcp::v4(), tcp_port)),
              udp_socket_(strand_.get_inner_executor(), {boost::asio::ip::udp::v4(), udp_port}),
              tcp_port_(tcp_port), udp_port_(udp_port), current_session_id_(0),
              dispatcher_(std::make_shared<UdpDispatcher>(udp_socket_, strand_)),
              session_controller_(std::move(session_controller)) {

        udp_socket_.set_option(boost::asio::socket_base::reuse_address(true));
        dispatcher_->startReceive();
        startAccept();
        initCoreRoutes();
    }

    void NetworkServer::removeSession(const uint32_t session_id) {
        post(strand_, [this, session_id] {
            if (session_controller_)
                session_controller_->onDisconnect(session_id);
            if (!sessions_.contains(session_id))
                return;

            sessions_.erase(session_id);
            std::cout << "[Server] Сессия с id=" << session_id << " удалена" << std::endl;
        });
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