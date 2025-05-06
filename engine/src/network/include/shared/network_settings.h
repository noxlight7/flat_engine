//
// Created by curo on 17.01.2025.
//

#ifndef NETWORK_SETTINGS_H
#define NETWORK_SETTINGS_H
#include <cstdint>
#include <boost/asio/buffer.hpp>
#include <boost/system/detail/error_code.hpp>

#ifdef _WIN32
  // минимизируем размер windows.h
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// для работы с сокетами в Windows
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#endif

namespace flat_engine::network {
    // константы заголовка сообщения
    typedef uint16_t MessageSize;
    typedef uint16_t MessageRouteType;
    typedef uint16_t SequenceID;
    // typedef uint32_t SessionMessageID;

    inline constexpr int k_message_len_pos = sizeof(MessageRouteType);
    // inline constexpr int k_message_id_pos = k_message_len_pos + sizeof(MessageSize);
    // inline constexpr int k_message_body_pos = k_message_id_pos + sizeof(SessionMessageID);
    inline constexpr int k_message_body_pos = k_message_len_pos + sizeof(MessageSize);
    static constexpr int k_header_size = sizeof(MessageRouteType) + sizeof(MessageSize);
    inline constexpr uint16_t k_max_packet_size = 0xFFFF;

    // inline constexpr uint16_t k_udp_header_size = sizeof(MessageRouteType) + sizeof(MessageSize);
    inline constexpr uint16_t k_max_udp_body_size = 1200;

    // Функция, которая вызывается при обработке пакета.
    // Возвращает true, если чтение должно быть продолжено
    using OnPacketReceived = std::function<bool (MessageRouteType, std::vector<uint8_t>&&)>;
    // Функция, которая вызывается при ошибке для её обработки
    using OnError = std::function<void (const boost::system::error_code& ec)>;


    struct UdpHeader {
        MessageRouteType route_id;
        MessageSize length;
        SequenceID sequence_id;

        static UdpHeader fromNetBuffer(const std::vector<uint8_t>& buffer) {
            auto const* fields = reinterpret_cast<const uint16_t*>(buffer.data());
            return {
                ntohs(fields[0]),
                ntohs(fields[1]),
                ntohs(fields[2])
            };
        }

        static UdpHeader fromArgsToNet(MessageRouteType route_id, MessageSize length, SequenceID sequence_id) {
            return {
                htons(route_id),
                htons(length),
                htons(sequence_id)
            };
        }
    };
    inline constexpr uint16_t k_max_udp_packet_size = k_max_udp_body_size + sizeof(UdpHeader);

    struct UdpMessage {
        UdpHeader header;
        std::vector<uint8_t> buffer;

        [[nodiscard]] auto buffers() const {
            return std::vector<boost::asio::const_buffer>{
                boost::asio::buffer(&header, sizeof(UdpHeader)),
                boost::asio::buffer(buffer)
            };
        }
    };

    struct TcpMessage {
        MessageRouteType route_id;
        MessageSize length;
        std::vector<uint8_t> buffer;

        [[nodiscard]] auto buffers() const {
            return std::vector<boost::asio::const_buffer>{
                boost::asio::buffer(&route_id, sizeof(route_id)),
                boost::asio::buffer(&length, sizeof(length)),
                boost::asio::buffer(buffer)
            };
        }
    };
}

#endif //NETWORK_SETTINGS_H
