//
// Created by curo on 28.04.2025.
//

#ifndef SERVER_GAME_DESERIALIZER_H
#define SERVER_GAME_DESERIALIZER_H
#include "deserializer.h"
#include "server_game_messages.hpp"
#include "routes.hpp"
namespace flat_engine::network {
    class GameDeserializeServerRoutes {
    public:
        static void initRoutes() {
            auto &deserializer = Deserializer::instance();
            deserializer.registerHandler(SERVER_SEND_PLAYER_AVATAR, PlayerAvatarMessage::create);
        }
    };
}
#endif //SERVER_GAME_DESERIALIZER_H
