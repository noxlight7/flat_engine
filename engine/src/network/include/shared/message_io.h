//
// Created by curo on 17.01.2025.
//

#ifndef MESSAGE_SENDER_H
#define MESSAGE_SENDER_H
#include <memory>
#include <queue>

#include "network_settings.h"
#include "transport.h"

namespace flat_engine::network {
    // Функция, которая вызывается при обработке пакета.
    // Возвращает true, если чтение должно быть продолжено
    using OnPacketReceived = std::function<bool (MessageRouteType, std::vector<uint8_t>&&)>;
    // Функция, которая вызывается при ошибке для её обработки
    using OnError = std::function<void (const boost::system::error_code& ec)>;

    class MessageIO: public std::enable_shared_from_this<MessageIO>{
        boost::asio::strand<boost::asio::any_io_executor> &strand_;
        std::unique_ptr<ITransport> transport_;
        OnPacketReceived on_packet_;
        OnError on_error_;
        std::vector<uint8_t> header_buffer_;
        std::vector<uint8_t> body_buffer_;
        std::queue<std::vector<uint8_t>> write_queue_;

        static constexpr int k_header_size = sizeof(MessageRouteType) + sizeof(MessageSize);

        void readHeader();
        void readBody(MessageRouteType route_id, MessageSize length);

    public:
        explicit MessageIO(
            std::unique_ptr<ITransport>&& transport,
            boost::asio::strand<boost::asio::any_io_executor> &strand,
            OnPacketReceived on_packet_received = nullptr,
            OnError on_error = nullptr);

        void setOnPacketReceived(OnPacketReceived on_packet_received) {
            on_packet_ = std::move(on_packet_received);
        }

        void setOnError(OnError on_error) {
            on_error_ = std::move(on_error);
        }

        void startReading() {
            readHeader();
        }

        void handleFatalError(const boost::system::error_code& ec);

        void close() {
            transport_->close();
        }

        void sendPacket(MessageRouteType route_id, const std::vector<uint8_t> &message);
        void queueMessage(std::vector<uint8_t> &&message);
        void doWrite();
    };
}

#endif //MESSAGE_SENDER_H
