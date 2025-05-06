//
// Created by curo on 20.01.2025.
//

#ifndef CONNECTION_H
#define CONNECTION_H
#include <utility>

#include "network_settings.h"
#include "transport.h"
namespace flat_engine::network {
    class IConnection : public std::enable_shared_from_this<IConnection> {
        OnError on_error_;
        OnPacketReceived on_packet_;
    public:
        virtual ITransport* getTransport() const = 0;
        explicit IConnection(OnError on_error, OnPacketReceived on_packet)
            : on_error_(std::move(on_error)),
              on_packet_(std::move(on_packet)) {}
        virtual ~IConnection() = default;
        // Начать асинхронное чтение buffer.size() байт в буфер buffer
        virtual void startRead() = 0;
        // Начать асинхронную отправку buffer
        virtual void asyncWrite(MessageRouteType route_id, std::vector<uint8_t> buffer,
            std::function<void (const boost::system::error_code&, size_t)> handler) = 0;
        // Закрыть транспорт (сокет)
        virtual void close() = 0;

        [[nodiscard]] OnError onError() const { return on_error_; }
        [[nodiscard]] OnPacketReceived onPacket() const { return on_packet_; }

        void handleFatalError(const boost::system::error_code& ec);
    };

    class TcpConnection : public IConnection {
        std::vector<uint8_t> header_buffer_;
        std::vector<uint8_t> body_buffer_;
        std::unique_ptr<ITransport> transport_;

        void readHeader();
        void readBody(MessageRouteType route_id, MessageSize length);
    public:
        explicit TcpConnection(std::unique_ptr<TcpTransport>&& transport, OnError on_error, OnPacketReceived on_packet)
            : IConnection(std::move(on_error), std::move(on_packet)),
              transport_(std::move(transport)),
              header_buffer_(k_header_size) {}

        void startRead() override;
        void asyncWrite(MessageRouteType route_id, std::vector<uint8_t> buffer,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;
        ITransport* getTransport() const override { return transport_.get(); }

        void close() override;
    };

    class UdpConnection : public IConnection {
        static SequenceID sequence_id_;
        std::unique_ptr<ITransport> transport_;

    public:
        bool readDataFrame(const std::vector<uint8_t>& buffer, uint16_t length);
        explicit UdpConnection(std::unique_ptr<ITransport> transport,
                               OnError on_error, OnPacketReceived on_packet);

        void startRead() override;
        void asyncWrite(MessageRouteType route_id, std::vector<uint8_t> buffer,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;
        ITransport* getTransport() const override { return transport_.get(); }

        void close() override;
    };
}
#endif //CONNECTION_H
