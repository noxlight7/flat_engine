//
// Created by curo on 17.01.2025.
//

#ifndef TRANSPORT_H
#define TRANSPORT_H
#include <cstdint>
#include <functional>
#include <vector>
#include <boost/asio.hpp>

namespace flat_engine::network {
    class ITransport {
    public:
        virtual ~ITransport() = default;
        // Начать асинхронное чтение buffer.size() байт в буфер buffer
        virtual void asyncRead(std::vector<uint8_t>& buffer,
            std::function<void (const boost::system::error_code&, size_t)> handler) = 0;
        // Начать асинхронную отправку buffer
        virtual void asyncWrite(const std::vector<uint8_t>& buffer,
            std::function<void (const boost::system::error_code&, size_t)> handler) = 0;

        virtual void close() = 0;
    };

    class TcpTransport : public ITransport {
        boost::asio::ip::tcp::socket socket_;

    public:
        explicit TcpTransport(boost::asio::ip::tcp::socket socket): socket_(std::move(socket)) {}

        TcpTransport(TcpTransport&& transport) noexcept: socket_(std::move(transport.socket_)) {}

        void asyncRead(std::vector<uint8_t>& buffer,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;

        void asyncWrite(const std::vector<uint8_t>& buffer,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;

        void close() override;
    };

    class UdpTransport : public ITransport {
        boost::asio::ip::udp::socket socket_;

    public:
        explicit UdpTransport(boost::asio::ip::udp::socket socket): socket_(std::move(socket)) {}

        UdpTransport(UdpTransport&& transport) noexcept: socket_(std::move(transport.socket_)) {}

        void asyncRead(std::vector<uint8_t>& buffer,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;

        void asyncWrite(const std::vector<uint8_t>& buffer,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;

        void close() override;
    };
}

#endif //TRANSPORT_H
