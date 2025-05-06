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
        const uint16_t udp_port,
        boost::asio::ip::udp::socket& udp_socket,
        std::unique_ptr<IGameData> game_data)
        : ISession(io_context, std::move(game_data), session_id),
          server_udp_port_(udp_port),
          udp_socket_(udp_socket),
          on_session_remove_(std::move(on_session_remove)) {
        auto on_receive = [this]
            (MessageRouteType route_id, std::vector<uint8_t>&& data) -> bool {
            return handlePacket(route_id, std::move(data));
        };
        auto on_error = [this](const boost::system::error_code&) {
            removeSession();
        };
        auto connection = std::make_shared<TcpConnection>(
            std::make_unique<TcpTransport>(
                std::move(socket)), std::move(on_error), std::move(on_receive));
        tcp_message_io_ = std::make_shared<MessageIO>(std::move(connection), getStrand());
    }

    void ServerSession::removeSession() {
        setDeleted();
        if (udp_message_io_) {
            udp_message_io_->close();
            udp_message_io_.reset();
        }
        tcp_message_io_->close();
        on_session_remove_(getSessionID());
    }

    void ServerSession::bindUdpEndpoint(
        boost::asio::ip::udp::endpoint udp_endpoint, UdpDispatcher& dispatcher) {
        auto on_receive = [this]
            (MessageRouteType route_id, std::vector<uint8_t>&& data) -> bool {
            return handlePacket(route_id, std::move(data));
        };
        auto on_error = [this](const boost::system::error_code&) {
            removeSession();
            udp_message_io_.reset();
        };
        auto udp_transport = std::make_unique<UdpTransport>(udp_socket_, std::move(udp_endpoint));
        auto udp_connection = std::make_shared<UdpConnection>(
            std::move(udp_transport), on_error, on_receive);
        dispatcher.addConnection(udp_connection);

        udp_message_io_ = std::make_shared<MessageIO>(
            std::move(udp_connection), getStrand());
        udp_message_io_->startReading();
    }
}
