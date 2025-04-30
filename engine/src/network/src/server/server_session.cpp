//
// Created by curo on 18.01.2025.
//

#include "server_session.h"

#include "deserializer.h"

namespace flat_engine::network {
    bool ServerSession::handlePacket(MessageRouteType route_id, std::vector<uint8_t>&& data) {
        auto result = Deserializer::instance().addMessage(route_id, std::move(data), shared_from_this());
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
        const uint32_t session_id,
        std::unique_ptr<IGameData> game_data)
        : ISession(io_context, std::move(game_data), session_id),
          message_io_(std::make_shared<MessageIO>(
              std::make_unique<TcpConnection>(std::make_unique<TcpTransport>(
                  std::move(socket))), getStrand())),
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
        setDeleted();
        message_io_->close();
        on_session_remove_(getSessionID());
    }

}
