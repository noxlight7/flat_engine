//
// Created by curo on 09.01.2025.
//

#ifndef ROUTES_H
#define ROUTES_H
#include <string>

#include "network_settings.h"
namespace flat_engine::network {
    constexpr std::string k_host_name = "localhost";
    constexpr uint16_t k_host_tcp_port = 5000;

    enum ServerRouteID: MessageRouteType {
        SERVER_LOGIN = 0x0001,
        SERVER_LOGOUT = 0x0002,
        SERVER_REGISTER = 0x0003,
        SERVER_LOGGING = 0x0004,
        SERVER_SEND_PLAYER_AVATAR = 0x0005,

        SHARED_TEST_TEXT_MESSAGE = 0x0201,

        MAX_ROUTE_ID = 0x02FF
    };

    // enum ClientRouteID: MessageRouteType {
    //     CLIENT_LOGIN = 0x0001,
    //     CLIENT_LOGOUT = 0x0002,
    //     CLIENT_REGISTER = 0x0003,
    //     CLIENT_LOGGING = 0x0004,
    //     CLIENT_SEND_TEXT_MESSAGE = 0x0005,
    // };
}

#endif //ROUTES_H
