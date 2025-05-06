//
// Created by curo on 01.01.2025.
//

#include <boost/asio.hpp>
#include <iostream>
#include <unistd.h>
#include <string>
#include <utility>
#include <sstream>
// #include <player_generated.h>
#include <flatbuffers/flatbuffers.h>

#include "deserializer.h"
#include "message_io.h"
#include "transport.h"

namespace flat_engine::network {
    constexpr int k_reconnect_time = 2000;

    class ClientSession: public ISession, public std::enable_shared_from_this<ClientSession> {
        boost::asio::io_context& io_context_;
        std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
        std::unique_ptr<boost::asio::ip::udp::socket> udp_socket_;
        boost::asio::ip::tcp::resolver resolver_;
        boost::asio::steady_timer timer_;
        std::string host_;
        unsigned short port_;
        unsigned short udp_port_;
        std::atomic<bool> is_stopped_;

        std::shared_ptr<MessageIO> tcp_message_io_;
        std::shared_ptr<MessageIO> udp_message_io_;
        // tcp::socket socket_();

    public:
        ClientSession(boost::asio::io_context& io_context,
            std::string  host,
            const unsigned short port,
            const unsigned short udp_port,
            std::unique_ptr<IGameData> game_data)
            : ISession(io_context, std::move(game_data), 1),
              io_context_(io_context), resolver_(io_context), is_stopped_(false),
              timer_(io_context), host_(std::move(host)), port_(port), udp_port_(udp_port) {
        }

        void start() {
            doResolve();
        }

        void doResolve() {
            auto self = shared_from_this();
            std::cout << "Connection to " << host_ << ":" << port_ << std::endl;
            resolver_.async_resolve(host_, std::to_string(port_),
                [this, self](const boost::system::error_code& ec,
                    const boost::asio::ip::tcp::resolver::results_type& results) {
                    std::cout << "Resolve done" << std::endl;
                    if (!ec) {
                        std::cout << "[Client] Resolve OK" << std::endl;
                        doConnect(results);
                    } else {
                        std::cerr << "[Client] Resolve Error: " << ec.message() << std::endl;
                        scheduleReconnect();
                    }
                });
        }

        void doConnect(const boost::asio::ip::tcp::resolver::results_type& endpoints) {
            socket_ = std::make_unique<boost::asio::ip::tcp::socket>(io_context_);

            auto self = shared_from_this();
            boost::asio::async_connect(*socket_, endpoints,
                [this, self, endpoints]
                (const boost::system::error_code& ec,
                    const boost::asio::ip::tcp::endpoint& ep)  {
                    if (!ec) {
                        auto ms = std::move(socket_);
                        onConnect(std::move(*ms), ep);
                    } else {
                        std::cerr << "[Client] Connect Error: " << ec.message() << std::endl;
                        scheduleReconnect();
                    }
                });
        }

        void connectByUdp() {
            udp_socket_ = make_unique<boost::asio::ip::udp::socket>(io_context_);
            udp_socket_->open(boost::asio::ip::udp::v4());
            udp_socket_->bind({ boost::asio::ip::udp::v4(), 0 });

            auto server_address = tcp_message_io_->getConnection()->getTransport()->getAddress();
            auto udp_server_endpoint = boost::asio::ip::udp::endpoint(server_address, udp_port_);
            udp_socket_->connect(udp_server_endpoint);

            uint16_t local_udp_port = udp_socket_->local_endpoint().port();

            uint16_t net_port = htons(local_udp_port);
            std::vector<uint8_t> buf(sizeof(net_port));
            std::memcpy(buf.data(), &net_port, sizeof(net_port));

            auto udp_tr = std::make_unique<UdpTransport>(*udp_socket_, udp_server_endpoint);
            auto onPacket = [this](MessageRouteType route, std::vector<uint8_t> &&data)->bool {
                return handleServerPacket(route, std::move(data));
            };

            auto onError = [this](const boost::system::error_code& ec) {
                return handleError(ec);
            };
            udp_message_io_ = std::make_shared<MessageIO>(
                std::make_shared<UdpConnection>(
                    std::move(udp_tr),
                    onError,
                    onPacket
                ),
                getStrand()
            );

            udp_message_io_->startReading();
            tcp_message_io_->sendPacket(CORE_CONNECT_UDP, std::move(buf));
        }

        void onConnect(boost::asio::ip::tcp::socket &&tcp_socket, const boost::asio::ip::tcp::endpoint& endpoint) {
            std::cout << "[Client] Клиент успешно подключён к " << endpoint << std::endl;

            auto transport = std::make_unique<TcpTransport>(std::move(tcp_socket));

            auto onPacket = [this](MessageRouteType route, std::vector<uint8_t> &&data)->bool {
                return handleServerPacket(route, std::move(data));
            };

            auto onError = [this](const boost::system::error_code& ec) {
                return handleError(ec);
            };

            tcp_message_io_ = std::make_shared<MessageIO>(std::make_shared<TcpConnection>(
                std::move(transport), onError, onPacket), getStrand());
            tcp_message_io_->startReading();

            connectByUdp();
        }

        void scheduleReconnect() {
            if (is_stopped_)
                return;

            auto self = shared_from_this();
            timer_.expires_after(std::chrono::milliseconds(k_reconnect_time));
            timer_.async_wait([this, self](const boost::system::error_code& ec) {
                handleTimer(ec);
            });
        }

        bool handleServerPacket(MessageRouteType route, std::vector<uint8_t>&& data) {
            auto res = Deserializer::instance().addMessage(
                route, std::move(data), shared_from_this());
            if (res != k_success) {
                scheduleReconnect();
                return false;
            }
            return true;
        }

        void handleError(boost::system::error_code ec) {
            std::cerr << "[Client] Ошибка MessageIO: " << ec.message() << std::endl;
            scheduleReconnect();
        }

        void sendReliable(MessageRouteType route_id, const std::vector<uint8_t>& buffer) {
            if (tcp_message_io_) {
                tcp_message_io_->sendPacket(route_id, buffer);
            } else {
                std::cerr << "[Client] Невозможно отправить пакет. Нет активного соединения" << std::endl;
            }
        }

        void sendFast(MessageRouteType route_id, const std::vector<uint8_t>& buffer) {
            if (udp_message_io_) {
                udp_message_io_->sendPacket(route_id, buffer);
            } else {
                std::cerr << "[Client] Невозможно отправить пакет. Нет активного соединения" << std::endl;
            }
        }

        void stop() {
            is_stopped_ = true;

            boost::system::error_code ec;
            timer_.cancel(ec);

            if (tcp_message_io_) {
                tcp_message_io_->close();
                tcp_message_io_.reset();
            }

            if (udp_message_io_) {
                udp_message_io_->close();
                udp_message_io_.reset();
            }
        }

        void handleTimer(const boost::system::error_code& ec) {
            if (!ec) {
                doResolve();
            }
        }
    };

    // class AsyncInput {
    //     boost::asio::io_context& io_context_;
    //     boost::asio::posix::stream_descriptor input_;
    //     boost::asio::streambuf buffer_;
    //     std::shared_ptr<ClientSession> async_client_;
    //
    //     static std::vector<std::string> getWords(const std::string& line) {
    //         std::istringstream iss(line);
    //         std::vector<std::string> words;
    //         std::string word;
    //         while (iss >> word) {
    //             words.emplace_back(word);
    //         }
    //         return words;
    //     }
    //
    //     static std::vector<uint8_t> readPlayer(const std::string& player_string) {
    //         const auto words = getWords(player_string);
    //         const auto name = words[0];
    //         return Serializer::serializePlayer(1, name, 1, 2, 3);
    //     }
    //
    //     static std::vector<uint8_t> readTextMessage(const std::string& text) {
    //         return Serializer::serializeTextMessage(text);
    //     }
    //
    //     void startRead() {
    //         async_read_until(input_, buffer_, "\n",
    //             [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
    //                 handleRead(ec, bytes_transferred);
    //             }
    //         );
    //     }
    //     void handleRead(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    //         if (!ec) {
    //             std::istream is(&buffer_);
    //             std::string line;
    //             getline(is, line);
    //             if (line == "q") {
    //                 std::cout << "Завершение программы." << std::endl;
    //                 io_context_.stop(); // Останавливаем io_context, что завершит программу
    //                 return;
    //             }
    //             async_client_->sendPacket(SERVER_SEND_TEXT_MESSAGE, readPlayer(line));
    //             startRead(); // Запускаем чтение снова
    //         } else {
    //             if (ec != boost::asio::error::operation_aborted) {
    //                 std::cerr << "Ошибка при чтении: " << ec.message() << std::endl;
    //                 io_context_.stop();
    //             }
    //         }
    //     }
    //
    // public:
    //     AsyncInput(boost::asio::io_context &io_context,
    //         std::shared_ptr<ClientSession> async_client)
    //         : io_context_(io_context), input_(io_context,
    //             ::dup(STDIN_FILENO)), async_client_(std::move(async_client)) {
    //         startRead();
    //     }
    // };

}

//using namespace flat_engine::network;
//
//int main(int argc, char *argv[]) {
//    boost::asio::io_context io_context;
//    auto client = std::make_shared<ClientSession>(io_context, "localhost", 5000);
//    client->start();
//    AsyncInput input(io_context, client);
//    io_context.run();
//}