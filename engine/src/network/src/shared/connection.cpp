//
// Created by curo on 20.01.2025.
//

#include "connection.h"

#include <iostream>
#include <memory>

namespace flat_engine::network {
    SequenceID UdpConnection::sequence_id_ = 0;

    void TcpConnection::startRead() {
        readHeader();
    }

    void TcpConnection::asyncWrite(MessageRouteType route_id, std::vector<uint8_t> buffer,
        std::function<void(const boost::system::error_code&, size_t)> handler) {

        auto message = std::make_shared<TcpMessage>(
            htons(route_id),
            htons(static_cast<uint16_t>(buffer.size())),
            std::move(buffer));

        getTransport()->asyncWrite(message->buffers(),
            [this, message, func=std::move(handler)](
            boost::system::error_code ec, size_t bytes_transferred) mutable {
            func(ec, bytes_transferred);
        });
    }

    void TcpConnection::close() {
        getTransport()->close();
    }

    void IConnection::handleFatalError(const boost::system::error_code& ec) {
        getTransport()->shutdown(boost::asio::socket_base::shutdown_both);
        getTransport()->cancel();
        if (on_error_) {
            on_error_(ec);
        }
        getTransport()->close();
    }

    void TcpConnection::readHeader() {
        auto self = shared_from_this();
        getTransport()->asyncRead(header_buffer_,
            [this, self](
                const boost::system::error_code& ec, size_t)  {
                if (!ec) {
                    MessageRouteType route_id;
                    std::memcpy(&route_id,
                                header_buffer_.data(),
                                sizeof(MessageSize)
                    );
                    route_id = ntohs(route_id);

                    MessageSize length;
                    std::memcpy(&length,
                                header_buffer_.data() + k_message_len_pos,
                                sizeof(MessageSize)
                    );
                    length = ntohs(length);

                    if (length > k_max_packet_size) {
                        std::cerr << "[Server] Размер сообщения выше "
                                     "максимально допустимого" << std::endl;
                        handleFatalError(boost::asio::error::message_size);
                        return;
                    }

                    readBody(route_id, length);
                } else {
                    handleFatalError(ec);
                }
            });
    }

    void TcpConnection::readBody(MessageRouteType route_id, const MessageSize length) {
        auto self = shared_from_this();  // Чтобы сессия не уничтожалась, пока идёт чтение
        body_buffer_.resize(length);

        getTransport()->asyncRead(body_buffer_,
            [this, self, route_id]
            (const boost::system::error_code& ec, std::size_t)
            {
                if (!ec) {
                    if (onPacket() && onPacket()(route_id, std::move(body_buffer_)))
                        readHeader();
                } else {
                    handleFatalError(ec);
                }
            });
    }

    bool UdpConnection::readDataFrame(const std::vector<uint8_t>& buffer, uint16_t length) {
        if (length < sizeof(UdpHeader))
            return false;

        UdpHeader header = UdpHeader::fromNetBuffer(buffer);
        if (length != sizeof(UdpHeader) + header.length)
            return false;

        std::vector<uint8_t> body_buffer_(header.length);
        std::memcpy(body_buffer_.data(),
            buffer.data() + sizeof(UdpHeader), header.length);
        if (onPacket())
            return onPacket()(header.route_id, std::move(body_buffer_));

        return false;
    }

    UdpConnection::UdpConnection(std::unique_ptr<ITransport> transport, OnError on_error,
        OnPacketReceived on_packet)
        : IConnection(std::move(on_error), std::move(on_packet)),
          transport_(std::move(transport)){
    }

    void UdpConnection::startRead() {
    }

    void UdpConnection::asyncWrite(MessageRouteType route_id, std::vector<uint8_t> buffer,
                                   std::function<void(const boost::system::error_code&, size_t)> handler) {
        auto message = std::make_shared<UdpMessage>(
            UdpHeader::fromArgsToNet(route_id, static_cast<MessageSize>(buffer.size()), ++sequence_id_),
            std::move(buffer));

        auto self = shared_from_this();
        getTransport()->asyncWrite(message->buffers(),
            [this, self, message, func=std::move(handler)](
            boost::system::error_code ec, size_t bytes_transferred) mutable {
                    func(ec, bytes_transferred);
        });
    }

    void UdpConnection::close() {
        getTransport()->close();
    }
}
