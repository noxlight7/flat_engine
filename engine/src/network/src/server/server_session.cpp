//
// Created by curo on 18.01.2025.
//

#include "server_session.h"

#include "deserializer.h"

namespace flat_engine::network {
    bool ServerSession::handlePacket(MessageRouteType route_id, std::vector<uint8_t>&& data) {
        auto result = Deserializer::instance().addMessage(route_id, std::move(data));
        if (result != k_success) {
            removeSession();
            return false;
        }
        return true;
    }

    void ServerSession::handleError(const boost::system::error_code& ec) {
        removeSession();
    }

    ServerSession::ServerSession(
        boost::asio::io_context &io_context,
        boost::asio::ip::tcp::socket&& socket,
        OnSessionRemove on_session_remove,
        const uint32_t session_id)
        : message_io_(std::make_shared<MessageIO>(
              io_context, std::make_unique<TcpTransport>(TcpTransport(std::move(socket))))),
          session_id_(session_id),
          on_session_remove_(std::move(on_session_remove)) {
        message_io_->setOnPacketReceived([this]
            (MessageRouteType route_id, std::vector<uint8_t>&& data) -> bool {
            return handlePacket(route_id, std::move(data));
        });
        message_io_->setOnError([this](const boost::system::error_code&) {
            removeSession();
        });
    }

    void ServerSession::removeSession() {
        message_io_->close();
        on_session_remove_(session_id_);
    }

}
