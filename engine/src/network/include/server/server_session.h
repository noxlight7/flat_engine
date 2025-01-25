//
// Created by curo on 18.01.2025.
//

#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H
#include "message_io.h"

namespace flat_engine::network {
    using OnSessionRemove = std::function<void (uint32_t session_id)>;
    class ServerSession: public std::enable_shared_from_this<ServerSession> {
        std::shared_ptr<MessageIO> message_io_;
        uint32_t session_id_;
        // uint32_t player_id_;
        // bool is_authorized_ = false;
        OnSessionRemove on_session_remove_;

        bool handlePacket(MessageRouteType route_id, std::vector<uint8_t>&& data);
        void handleError(const boost::system::error_code& ec);

    public:
        ServerSession(
            boost::asio::io_context &io_context,
            boost::asio::ip::tcp::socket &&socket,
            OnSessionRemove on_session_remove,
            uint32_t session_id);

        void removeSession();

        void start() const {
            message_io_->startReading();
        }
        void sendReliable(MessageRouteType route_id, const std::vector<uint8_t>& data) {
            message_io_->sendPacket(route_id, data);
        }
        void sendFast(MessageRouteType route_id, const std::vector<uint8_t>& data) {
            sendReliable(route_id, data);
        }
    };
}
#endif //SERVER_SESSION_H
