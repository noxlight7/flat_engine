//
// Created by curo on 17.01.2025.
//

#ifndef MESSAGE_SENDER_H
#define MESSAGE_SENDER_H
#include <memory>
#include <queue>

#include "network_settings.h"
#include "connection.h"

namespace flat_engine::network {
    class MessageIO: public std::enable_shared_from_this<MessageIO>{
        boost::asio::strand<boost::asio::any_io_executor> &strand_;
        std::shared_ptr<IConnection> transport_;
        std::queue<std::pair<MessageRouteType, std::vector<uint8_t>>> write_queue_;

        void readNextMessage();

    public:
        explicit MessageIO(
            std::shared_ptr<IConnection>&& transport,
            boost::asio::strand<boost::asio::any_io_executor> &strand);

        // void setOnPacketReceived(OnPacketReceived on_packet_received) {
        //     on_packet_ = std::move(on_packet_received);
        // }
        //
        // void setOnError(OnError on_error) {
        //     on_error_ = std::move(on_error);
        // }

        void startReading() {
            transport_->startRead();
        }

        void close() {
            transport_->close();
        }

        void sendPacket(MessageRouteType route_id, std::vector<uint8_t> message);
        void queueMessage(MessageRouteType route_id, std::vector<uint8_t> &&message);
        void doWrite();

        IConnection* getConnection() const {return transport_.get();}
    };
}

#endif //MESSAGE_SENDER_H
