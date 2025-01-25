#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <deque>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <cstdint>

//----------------------------------------
// Константы портов
//----------------------------------------
static constexpr unsigned short TCP_PORT = 5000;
static constexpr unsigned short UDP_PORT = 5001;

//----------------------------------------
// Утилиты для (де)сериализации 4-байтного заголовка
//----------------------------------------
inline uint32_t fromNetworkBytes(const uint8_t* data)
{
    return (uint32_t(data[0]) << 24) |
           (uint32_t(data[1]) << 16) |
           (uint32_t(data[2]) << 8)  |
            uint32_t(data[3]);
}
inline void toNetworkBytes(uint32_t value, uint8_t* out)
{
    out[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    out[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    out[2] = static_cast<uint8_t>((value >> 8 ) & 0xFF);
    out[3] = static_cast<uint8_t>( value        & 0xFF);
}

// Forward-декларация
class NetworkClient;

//----------------------------------------
// Класс NetworkClient
//----------------------------------------
class NetworkClient
{
public:
    NetworkClient(boost::asio::io_context& io_context,
                  const std::string& server_host)
        : io_context_(io_context)
        , tcp_socket_(io_context)
        , udp_socket_(io_context)
        , resolver_(io_context)
        , server_host_(server_host)
    {
    }

    void start()
    {
        // 1) Подключаемся по TCP
        auto endpoints = resolver_.resolve(server_host_, std::to_string(TCP_PORT));
        boost::asio::async_connect(
            tcp_socket_,
            endpoints,
            [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint)
            {
                if (!ec) {
                    std::cout << "[Client] TCP connected to "
                              << server_host_ << ":" << TCP_PORT << "\n";
                    do_readHeader();
                } else {
                    std::cerr << "[Client] TCP connect failed: " << ec.message() << "\n";
                }
            }
        );

        // 2) Открываем локальный UDP-сокет (порт = 0 => эпhemeral)
        udp_socket_.open(boost::asio::ip::udp::v4());
        udp_socket_.bind({boost::asio::ip::udp::v4(), 0});
        std::cout << "[Client] Local UDP bound to "
                  << udp_socket_.local_endpoint().port() << "\n";

        // Начинаем читать UDP (приём пакетов от сервера)
        do_readUdp();
    }

    // Отправка по TCP (length-based)
    void sendGuaranteed(const std::vector<uint8_t>& data)
    {
        // Формируем [4 байта длины] + [payload]
        std::vector<uint8_t> packet(4 + data.size());
        toNetworkBytes(static_cast<uint32_t>(data.size()), &packet[0]);
        std::copy(data.begin(), data.end(), packet.begin() + 4);

        bool writing = !write_queue_.empty();
        write_queue_.push_back(std::move(packet));
        if (!writing) {
            do_write();
        }
    }

    // Отправка по UDP (просто один датаграм)
    void sendFast(const std::vector<uint8_t>& data)
    {
        // шлём на server_host_:UDP_PORT
        boost::asio::ip::udp::endpoint server_ep(
            boost::asio::ip::make_address(server_host_),
            UDP_PORT
        );
        udp_socket_.async_send_to(
            boost::asio::buffer(data),
            server_ep,
            [](auto /*ec*/, auto /*bytes*/) {}
        );
    }

    // Закрыть сокеты
    void close()
    {
        boost::asio::post(io_context_, [this]() {
            tcp_socket_.close();
            udp_socket_.close();
        });
    }

private:
    //----------------------------------------
    // Чтение TCP по длине
    //----------------------------------------
    void do_readHeader()
    {
        auto self = this;
        boost::asio::async_read(
            tcp_socket_,
            boost::asio::buffer(header_buf_),
            [this, self](boost::system::error_code ec, std::size_t /*bytes*/)
            {
                if (!ec) {
                    uint32_t length = fromNetworkBytes(header_buf_.data());
                    do_readBody(length);
                } else {
                    std::cerr << "[Client] do_readHeader error: " << ec.message() << "\n";
                    tcp_socket_.close();
                }
            }
        );
    }

    void do_readBody(std::size_t length)
    {
        if (length == 0) {
            onMessageReceived({});
            return do_readHeader();
        }

        body_buf_.resize(length);
        auto self = this;
        boost::asio::async_read(
            tcp_socket_,
            boost::asio::buffer(body_buf_),
            [this, self](boost::system::error_code ec, std::size_t /*bytes*/)
            {
                if (!ec) {
                    onMessageReceived(body_buf_);
                    do_readHeader();
                } else {
                    std::cerr << "[Client] do_readBody error: " << ec.message() << "\n";
                    tcp_socket_.close();
                }
            }
        );
    }

    void onMessageReceived(const std::vector<uint8_t>& data)
    {
        std::cout << "[Client] TCP message size=" << data.size() << "\n";
    }

    //----------------------------------------
    // Запись TCP (length-based)
    //----------------------------------------
    void do_write()
    {
        auto self = this;
        boost::asio::async_write(
            tcp_socket_,
            boost::asio::buffer(write_queue_.front()),
            [this, self](boost::system::error_code ec, std::size_t /*bytes*/)
            {
                if (!ec) {
                    write_queue_.pop_front();
                    if (!write_queue_.empty()) {
                        do_write();
                    }
                } else {
                    std::cerr << "[Client] do_write error: " << ec.message() << "\n";
                    tcp_socket_.close();
                }
            }
        );
    }

    //----------------------------------------
    // Чтение UDP
    //----------------------------------------
    void do_readUdp()
    {
        udp_socket_.async_receive_from(
            boost::asio::buffer(udp_buf_),
            sender_ep_,
            [this](boost::system::error_code ec, std::size_t bytes_recvd)
            {
                if (!ec) {
                    std::vector<uint8_t> msg(udp_buf_.begin(),
                                             udp_buf_.begin() + bytes_recvd);
                    std::cout << "[Client] UDP from server => size=" << msg.size() << "\n";

                    // Повторяем чтение
                    do_readUdp();
                } else {
                    std::cerr << "[Client] UDP read error: " << ec.message() << "\n";
                }
            }
        );
    }

private:
    boost::asio::io_context& io_context_;

    // TCP
    boost::asio::ip::tcp::socket tcp_socket_;
    // Для чтения TCP
    std::array<uint8_t, 4> header_buf_{};
    std::vector<uint8_t> body_buf_;
    // Для записи TCP
    std::deque<std::vector<uint8_t>> write_queue_;

    // UDP
    boost::asio::ip::udp::socket udp_socket_;
    std::array<uint8_t, 2048> udp_buf_{};
    boost::asio::ip::udp::endpoint sender_ep_;

    // Резолвер (TCP connect)
    boost::asio::ip::tcp::resolver resolver_;
    std::string server_host_;
};
