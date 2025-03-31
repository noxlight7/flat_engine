//
// Created by curo on 08.01.2025.
//

#ifndef ASYNC_SERVER_H
#define ASYNC_SERVER_H
#include <boost/asio.hpp>
#include "server_session.h"
#include "session_controller.h"

namespace flat_engine::network {

    class NetworkServer {
        boost::asio::io_context& io_context_;
        boost::asio::strand<boost::asio::any_io_executor> strand_;
        boost::asio::ip::tcp::acceptor acceptor_;
        uint16_t port_;

        uint32_t current_session_id_;
        std::unordered_map<uint32_t, std::shared_ptr<ServerSession>> sessions_;

        std::unique_ptr<ISessionController> session_controller_;

        uint32_t nextSessionId();

        void startAccept();

    public:
        NetworkServer(
            boost::asio::io_context& io_context,
            uint16_t port,
            std::unique_ptr<ISessionController> session_controller);

        void removeSession(uint32_t session_id);

        // std::shared_ptr<ServerSession> getSession(uint32_t session_id) { return sessions_.at(session_id); }
    };
}
#endif //ASYNC_SERVER_H
