//
// Created by curo on 08.01.2025.
//

#ifndef ASYNC_SERVER_H
#define ASYNC_SERVER_H
#include <boost/asio.hpp>
#include "server_session.h"
#include "session_controller.h"
#include "udp_dispatcher.h"

namespace flat_engine::network {

    class NetworkServer {
        boost::asio::io_context& io_context_;
        boost::asio::strand<boost::asio::any_io_executor> strand_;
        boost::asio::ip::tcp::acceptor acceptor_;
        std::vector<uint8_t> rx_buffer_;
        uint16_t tcp_port_;
        uint16_t udp_port_;
        std::shared_ptr<UdpDispatcher> dispatcher_;

        uint32_t current_session_id_;
        std::unordered_map<uint32_t, std::shared_ptr<ServerSession>> sessions_;
        boost::asio::ip::udp::socket udp_socket_;

        std::unique_ptr<ISessionController> session_controller_;

        uint32_t nextSessionId();

        void startAccept();

        void initCoreRoutes();

    public:
        NetworkServer(
            boost::asio::io_context& io_context,
            uint16_t tcp_port,
            uint16_t udp_port,
            std::unique_ptr<ISessionController> session_controller);

        void removeSession(uint32_t session_id);

        // std::shared_ptr<ServerSession> getSession(uint32_t session_id) { return sessions_.at(session_id); }
    };
}
#endif //ASYNC_SERVER_H
