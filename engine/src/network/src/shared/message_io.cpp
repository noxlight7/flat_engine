//
// Created by curo on 17.01.2025.
//

#include "message_io.h"

#include <utility>
#include <iostream>

namespace flat_engine::network {


    void MessageIO::readNextMessage() {
    }

    MessageIO::MessageIO(
        std::shared_ptr<IConnection>&& transport,
        boost::asio::strand<boost::asio::any_io_executor> &strand)
        : strand_(strand),
          transport_(std::move(transport)){
    }

    void MessageIO::sendPacket(const MessageRouteType route_id, std::vector<uint8_t> message) {
        queueMessage(route_id, std::move(message));
    }

    void MessageIO::queueMessage(const MessageRouteType route_id, std::vector<uint8_t>&& message) {
        auto self = shared_from_this();

        boost::asio::post(strand_, [this, self, route_id, msg = std::move(message)]() mutable {
            const bool was_empty = write_queue_.empty();
            write_queue_.emplace(route_id, std::move(msg));

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
        const auto& [route_id, front_message] = write_queue_.front();
        transport_->asyncWrite(route_id, front_message,
            [this, self, front_message](const boost::system::error_code& ec, size_t) {
                if (!ec) {
                    write_queue_.pop();

                    if (!write_queue_.empty()) {
                        doWrite();
                    }
                } else {
                    std::cerr << "Ошибка записи: " << ec.message() << std::endl;
                    transport_->handleFatalError(ec);
                }
            });
    }
}
