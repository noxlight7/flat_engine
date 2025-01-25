//
// Created by curo on 13.01.2025.
//

#ifndef GAME_DESERIALIZER_H
#define GAME_DESERIALIZER_H

#include "deserializer.h"
#include "game_messages.hpp"
#include "routes.hpp"
namespace flat_engine::network {
    class GameDeserializeRoutes {
    public:
        static void initRoutes() {
            auto &deserializer = Deserializer::instance();
            deserializer.registerHandler(SERVER_SEND_PLAYER_AVATAR, PlayerAvatarMessage::create);
            deserializer.registerHandler(SHARED_TEST_TEXT_MESSAGE, TextMessage::create);
        }
    };
}
#endif //GAME_DESERIALIZER_H
