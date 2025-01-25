//
// Created by curo on 17.01.2025.
//

#ifndef NETWORK_SETTINGS_H
#define NETWORK_SETTINGS_H
#include <cstdint>

namespace flat_engine::network {
    // константы заголовка сообщения
    typedef uint16_t MessageSize;
    typedef uint16_t MessageRouteType;
    // typedef uint32_t SessionMessageID;

    inline constexpr int k_message_len_pos = sizeof(MessageRouteType);
    // inline constexpr int k_message_id_pos = k_message_len_pos + sizeof(MessageSize);
    // inline constexpr int k_message_body_pos = k_message_id_pos + sizeof(SessionMessageID);
    inline constexpr int k_message_body_pos = k_message_len_pos + sizeof(MessageSize);
    inline constexpr uint16_t k_max_packet_size = 0xFFFF;
}

#endif //NETWORK_SETTINGS_H
