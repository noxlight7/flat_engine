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
        virtual void asyncWrite(const std::vector<boost::asio::const_buffer>& buffers,
            std::function<void (const boost::system::error_code&, size_t)> handler) = 0;
        virtual void shutdown(boost::asio::socket_base::shutdown_type what) = 0;
        [[nodiscard]] virtual boost::asio::ip::address getAddress() const = 0;
        virtual void cancel() = 0;
        virtual void close() = 0;

        [[nodiscard]] virtual boost::asio::ip::tcp::endpoint peerEndpoint() const { return {}; }
        [[nodiscard]] virtual boost::asio::ip::udp::endpoint lastSender() const { return {}; }
    };

    class TcpTransport : public ITransport {
        boost::asio::ip::tcp::socket socket_;

    public:
        explicit TcpTransport(boost::asio::ip::tcp::socket socket): socket_(std::move(socket)) {}

        TcpTransport(TcpTransport&& transport) noexcept: socket_(std::move(transport.socket_)) {}

        [[nodiscard]] boost::asio::ip::address getAddress() const override {
            return (socket_.remote_endpoint().address());
        }

        void asyncRead(std::vector<uint8_t>& buffer,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;

        void asyncWrite(const std::vector<boost::asio::const_buffer>& buffers,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;

        void shutdown(boost::asio::socket_base::shutdown_type what) override;
        void cancel() override;

        void close() override;

        [[nodiscard]] boost::asio::ip::tcp::endpoint peerEndpoint() const override {
            return socket_.remote_endpoint();
        }
    };

    class UdpTransport : public ITransport {
        boost::asio::ip::udp::socket &socket_;
        boost::asio::ip::udp::endpoint endpoint_;

    public:
        explicit UdpTransport(boost::asio::ip::udp::socket &socket,
            boost::asio::ip::udp::endpoint endpoint)
            : socket_(socket), endpoint_(std::move(endpoint)) {
        }

        UdpTransport(UdpTransport&& transport) noexcept
            : socket_(transport.socket_), endpoint_(std::move(transport.endpoint_)) {}

        void asyncRead(std::vector<uint8_t>& buffer,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;

        void asyncWrite(const std::vector<boost::asio::const_buffer>& buffers,
            std::function<void(const boost::system::error_code&, size_t)> handler) override;

        void shutdown(boost::asio::socket_base::shutdown_type what) override {}
        void cancel() override;
        void close() override;

        [[nodiscard]] boost::asio::ip::udp::endpoint lastSender() const override {
            return endpoint_;
        }

        [[nodiscard]] boost::asio::ip::address getAddress() const override {
            return socket_.remote_endpoint().address();
        }

        [[nodiscard]] boost::asio::ip::udp::endpoint getEndpoint() const {
            return endpoint_;
        }

        void open() {
            socket_.open(boost::asio::ip::udp::v4());
        }
        void bind(const boost::asio::ip::udp::endpoint& ep) {
            socket_.bind(ep);
        }
        void connect(const boost::asio::ip::udp::endpoint& ep) {
            socket_.connect(ep);
        }
    };
}

#endif //TRANSPORT_H
