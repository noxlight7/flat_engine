//
// Created by curo on 28.04.2025.
//

#ifndef PLAYER_DATA_H
#define PLAYER_DATA_H
#include <cstdint>
#include <string>

#include "session.h"

struct PlayerData : flat_engine::network::IGameData {
    uint32_t player_id{};
    uint32_t session_id{};
    std::string login;
    PlayerData() = default;
    PlayerData(int player_id, uint32_t session_id, std::string login)
        : player_id(player_id), session_id(session_id), login(std::move(login)) {

    }
};

#endif //PLAYER_DATA_H
