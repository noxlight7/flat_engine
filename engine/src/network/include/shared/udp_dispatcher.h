//
// Created by curo on 03.05.2025.
//

#ifndef UDP_DISPATCHER_H
#define UDP_DISPATCHER_H
#include <iostream>
#include <memory>
#include <utility>

#include "connection.h"
#include "network_settings.h"

namespace flat_engine::network {
    struct EndpointHash {
        std::size_t operator()(const boost::asio::ip::udp::endpoint& ep) const noexcept
        {
            std::size_t h1 = std::hash<std::string>()(ep.address().to_string());
            std::size_t h2 = std::hash<unsigned short>()(ep.port());
            return h1 ^ (h2 << 1);
        }
    };

    class UdpDispatcher : public std::enable_shared_from_this<UdpDispatcher> {
        using ConnPtr = std::shared_ptr<UdpConnection>;
        boost::asio::ip::udp::socket& socket_;
        boost::asio::strand<boost::asio::any_io_executor>& strand_;
        std::unordered_map<boost::asio::ip::udp::endpoint,
                           std::weak_ptr<UdpConnection>,
                           EndpointHash> sessions_;
        std::vector<std::uint8_t> rx_;
        boost::asio::ip::udp::endpoint remote_;

        void doReceive() {
            rx_.resize(k_max_udp_packet_size);
            auto self = shared_from_this();
            socket_.async_receive_from(
                boost::asio::buffer(rx_), remote_,
                bind_executor(strand_,
                    [self](auto ec, std::size_t n) {
                          if (!ec && !self->dispatch(n))
                              std::cerr << "Dispatch failed" << std::endl;
                          else if (ec != boost::asio::error::operation_aborted)
                              self->doReceive();
                  }));
        }

        bool dispatch(std::size_t n) {
            auto it = sessions_.find(remote_);
            if (it != sessions_.end()) {
                ConnPtr conn = it->second.lock();
                return conn->readDataFrame(rx_, n);
            }
            return false;
        }

    public:
        void addConnection(std::shared_ptr<UdpConnection>& connection) {
            sessions_[dynamic_cast<UdpTransport*>(connection->getTransport())->getEndpoint()] = connection;
        }

        explicit UdpDispatcher(boost::asio::ip::udp::socket &socket,
                              boost::asio::strand<boost::asio::any_io_executor>& strand)
            : socket_(socket),
              strand_(strand) {}

        void startReceive() {
            doReceive();
        }
    };
}
#endif //UDP_DISPATCHER_H
