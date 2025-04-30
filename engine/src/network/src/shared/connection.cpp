//
// Created by curo on 20.01.2025.
//

#include "connection.h"
namespace flat_engine::network {
    void TcpConnection::asyncRead(std::vector<uint8_t>& buffer,
        std::function<void(const boost::system::error_code&, size_t)> handler) {
        getTransport()->asyncRead(buffer, handler);
    }

    void TcpConnection::asyncWrite(const std::vector<uint8_t>& buffer,
        std::function<void(const boost::system::error_code&, size_t)> handler) {
        getTransport()->asyncWrite(buffer, handler);
    }

    void TcpConnection::close() {
        getTransport()->close();
    }

    void UdpConnection::close() {
        getTransport()->close();
    }
}