//
// Created by curo on 17.01.2025.
//

#include "transport.h"

namespace flat_engine::network {
    void TcpTransport::asyncRead(std::vector<uint8_t>& buffer,
        std::function<void(const boost::system::error_code&, size_t)> handler) {
        boost::asio::async_read(socket_, boost::asio::buffer(buffer), handler);
    }

    void TcpTransport::asyncWrite(const std::vector<boost::asio::const_buffer>& buffers,
        std::function<void(const boost::system::error_code&, size_t)> handler) {
        boost::asio::async_write(socket_, buffers, handler);
    }

    void TcpTransport::shutdown(boost::asio::socket_base::shutdown_type what) {
        boost::system::error_code ec;
        socket_.shutdown(what, ec);
    }

    void TcpTransport::cancel() {
        boost::system::error_code ec;
        socket_.cancel(ec);
    }

    void TcpTransport::close() {
        boost::system::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        socket_.close(ec);
    }

    void UdpTransport::asyncRead(std::vector<uint8_t>& buffer,
        std::function<void(const boost::system::error_code&, size_t)> handler) {
        socket_.async_receive_from(boost::asio::buffer(buffer), endpoint_, std::move(handler));
    }

    void UdpTransport::asyncWrite(const std::vector<boost::asio::const_buffer>& buffers,
        std::function<void(const boost::system::error_code&, size_t)> handler) {
        socket_.async_send_to(buffers, endpoint_, std::move(handler));
    }

    void UdpTransport::cancel() {
        boost::system::error_code ec;
        socket_.cancel(ec);
    }

    void UdpTransport::close() {
        boost::system::error_code ec;
        socket_.cancel(ec);
        socket_.close(ec);
    }


}
