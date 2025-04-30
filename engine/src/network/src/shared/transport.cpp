//
// Created by curo on 17.01.2025.
//

#include "transport.h"

namespace flat_engine::network {
    void TcpTransport::asyncRead(std::vector<uint8_t>& buffer,
        std::function<void(const boost::system::error_code&, size_t)> handler) {
        boost::asio::async_read(socket_, boost::asio::buffer(buffer), handler);
    }

    void TcpTransport::asyncWrite(const std::vector<uint8_t>& buffer,
        std::function<void(const boost::system::error_code&, size_t)> handler) {
        boost::asio::async_write(socket_, boost::asio::buffer(buffer), handler);
    }

    void TcpTransport::close() {
        boost::system::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        socket_.close(ec);
    }

    void UdpTransport::asyncRead(std::vector<uint8_t>& buffer,
        std::function<void(const boost::system::error_code&, size_t)> handler) {
        socket_.async_receive(boost::asio::buffer(buffer), handler);
    }

    void UdpTransport::asyncWrite(const std::vector<uint8_t>& buffer,
        std::function<void(const boost::system::error_code&, size_t)> handler) {
        socket_.async_send(boost::asio::buffer(buffer), handler);
    }

    void UdpTransport::close() {
        boost::system::error_code ec;
        socket_.close(ec);
    }
}
