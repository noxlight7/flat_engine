//
// Created by curo on 28.04.2025.
//

#ifndef PLAYER_AVATAR_MESSAGE_H
#define PLAYER_AVATAR_MESSAGE_H
#include <iostream>
#include <player_avatar_generated.h>
#include "messages.h"

namespace flat_engine::network {
    class PlayerAvatarMessage final : public IMessage {
        const Game::PlayerAvatarT* player_;
    public:
        explicit PlayerAvatarMessage(std::vector<uint8_t>&& data, std::shared_ptr<ISession> session)
            : IMessage(std::move(data), std::move(session)) {
            // todo: логика преобразования сырого буфера в полезные данные
            player_ = nullptr;
            if (auto verifier = flatbuffers::Verifier(data_.data(), data_.size());
                !Game::VerifyPlayerAvatarTBuffer(verifier)) {
                is_verified = false;
                player_ = nullptr;
                return;
                }

            player_ = Game::GetPlayerAvatarT(data_.data());
            is_verified = player_ != nullptr;
        }

        static IMessage* create(std::vector<uint8_t>&& data, std::shared_ptr<ISession> session) {
            return new PlayerAvatarMessage(std::move(data), std::move(session));
        }

        void process() override {
            float x = player_->player_info()->position().in_cell_coords().x();
            float y = player_->player_info()->position().in_cell_coords().y();
            int cell_x = player_->player_info()->position().cell_coords().x();
            int cell_y = player_->player_info()->position().cell_coords().y();
            float speed_x = player_->player_info()->speed().x();
            float speed_y = player_->player_info()->speed().y();

            std::cout << "[" << cell_x << ";" << cell_y << ":" << x << ";" << y <<
                "(" << speed_x << ";" << speed_y << ")]" << std::endl;
            // std::cout << "Player Name: " << player_->name()->c_str() << std::endl;
            // const Game::Vec3* position = player_->position();
            // std::cout << "Position: ("
            //           << position->x() << ", "
            //           << position->y() << ", "
            //           << position->z() << ")" << std::endl;


        }
    };
}
#endif //PLAYER_AVATAR_MESSAGE_H
