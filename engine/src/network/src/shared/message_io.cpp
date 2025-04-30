//
// Created by curo on 17.01.2025.
//

#include "message_io.h"

#include <utility>
#include <iostream>

namespace flat_engine::network {
    void MessageIO::readHeader() {
        auto self = shared_from_this();
        transport_->asyncRead(header_buffer_,
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

    void MessageIO::readBody(MessageRouteType route_id, const MessageSize length) {
        auto self = shared_from_this();  // Чтобы сессия не уничтожалась, пока идёт чтение
        body_buffer_.resize(length);

        transport_->asyncRead(body_buffer_,
            [this, self, route_id]
            (const boost::system::error_code& ec, std::size_t)
            {
                if (!ec) {
                    if (on_packet_(route_id, std::move(body_buffer_)))
                        readHeader();
                } else {
                    handleFatalError(ec);
                }
            });
    }

    MessageIO::MessageIO(
        std::unique_ptr<IConnection>&& transport,
        boost::asio::strand<boost::asio::any_io_executor> &strand,
        OnPacketReceived on_packet_received,
        OnError on_error)
        : strand_(strand),
          transport_(std::move(transport)),
          on_packet_(std::move(on_packet_received)),
          on_error_(std::move(on_error)),
          header_buffer_(k_header_size){
    }

    void MessageIO::handleFatalError(const boost::system::error_code& ec) {
        if (on_error_) {
            on_error_(ec);
        }
        close();
    }

    void MessageIO::sendPacket(const MessageRouteType route_id, const std::vector<uint8_t>& message) {
        std::vector<uint8_t> packet;
        packet.resize(k_message_body_pos + message.size());

        const MessageRouteType _route_id = htons(route_id);
        std::memcpy(packet.data(), &_route_id, sizeof(MessageSize));

        const MessageSize length = htons(message.size());
        std::memcpy(packet.data() + k_message_len_pos, &length, sizeof(MessageSize));

        std::memcpy(packet.data() + k_message_body_pos, message.data(), message.size());

        queueMessage(std::move(packet));
    }

    void MessageIO::queueMessage(std::vector<uint8_t>&& message) {
        auto self = shared_from_this();

        boost::asio::post(strand_, [this, self, msg = std::move(message)]() mutable {
            const bool was_empty = write_queue_.empty();
            write_queue_.push(std::move(msg));

            if (was_empty) {
                doWrite();
            }
        });
    }

    void MessageIO::doWrite() {
        if (write_queue_.empty()) {
            return;
        }

        auto self = shared_from_this();
        const auto& front_message = write_queue_.front();
        transport_->asyncWrite(front_message,
            [this, self, front_message](const boost::system::error_code& ec, size_t) {
                if (!ec) {
                    write_queue_.pop();

                    if (!write_queue_.empty()) {
                        doWrite();
                    }
                } else {
                    std::cerr << "[Server] Ошибка записи: " << ec.message() << std::endl;
                    handleFatalError(ec);
                }
            });
    }
}
