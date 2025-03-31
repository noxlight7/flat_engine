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
        boost::asio::ip::tcp::resolver resolver_;
        boost::asio::steady_timer timer_;
        std::string host_;
        unsigned short port_;
        std::atomic<bool> is_stopped_;

        std::shared_ptr<MessageIO> message_io_;
        // tcp::socket socket_();

    public:
        ClientSession(boost::asio::io_context& io_context,
            std::string  host,
            const unsigned short port,
            std::unique_ptr<IGameData> game_data)
            : ISession(io_context, std::move(game_data), 1),
              io_context_(io_context), resolver_(io_context), is_stopped_(false),
              timer_(io_context), host_(std::move(host)), port_(port) {
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

        void onConnect(boost::asio::ip::tcp::socket &&socket, const boost::asio::ip::tcp::endpoint& endpoint) {
            std::cout << "[Client] Клиент успешно подключён к " << endpoint << std::endl;

            auto transport = std::make_unique<TcpTransport>(std::move(socket));

            auto onPacket = [this](MessageRouteType route, std::vector<uint8_t> &&data)->bool {
                return handleServerPacket(route, std::move(data));
            };

            auto onError = [this](const boost::system::error_code& ec) {
                return handleError(ec);
            };

            message_io_ = std::make_shared<MessageIO>(std::move(transport), getStrand(), onPacket, onError);
            message_io_->startReading();
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

        void sendPacket(MessageRouteType route_id, const std::vector<uint8_t>& buffer) {
            if (message_io_) {
                message_io_->sendPacket(route_id, buffer);
            } else {
                std::cerr << "[Client] Невозможно отправить пакет. Нет активного соединения" << std::endl;
            }
        }

        void stop() {
            is_stopped_ = true;

            boost::system::error_code ec;
            timer_.cancel(ec);

            if (message_io_) {
                message_io_->close();
                message_io_.reset();
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