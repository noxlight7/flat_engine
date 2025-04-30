//
// Created by curo on 20.01.2025.
//

#ifndef CONNECTION_H
#define CONNECTION_H
#include "transport.h"
namespace flat_engine::network {
    class IConnection {
        std::unique_ptr<ITransport> transport_;
    public:
        [[nodiscard]] ITransport* getTransport() const { return transport_.get(); }
        explicit IConnection(std::unique_ptr<ITransport> transport): transport_(std::move(transport)){}
        virtual ~IConnection() = default;
        // Начать асинхронное чтение buffer.size() байт в буфер buffer
        virtual void asyncRead(std::vector<uint8_t>& buffer,
            std::function<void (const boost::system::error_code&, size_t)> handler) = 0;
        // Начать асинхронную отправку buffer
        virtual void asyncWrite(const std::vector<uint8_t>& buffer,
            std::function<void (const boost::system::error_code&, size_t)> handler) = 0;
        // Закрыть транспорт (сокет)
        virtual void close() = 0;
    };

    class TcpConnection : public IConnection {
    public:
        explicit TcpConnection(std::unique_ptr<TcpTransport>&& transport): IConnection(std::move(transport)) {}

        void asyncRead(std::vector<uint8_t>& buffer,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;

        void asyncWrite(const std::vector<uint8_t>& buffer,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;

        void close() override;
    };

    class UdpConnection : public IConnection {
    public:
        explicit UdpConnection(std::unique_ptr<TcpTransport>&& transport): IConnection(std::move(transport)) {}

        void asyncRead(std::vector<uint8_t>& buffer,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;

        void asyncWrite(const std::vector<uint8_t>& buffer,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;

        void close() override;
    };
}
#endif //CONNECTION_H
