//
// Created by curo on 13.01.2025.
//

#ifndef GAME_MESSAGES_H
#define GAME_MESSAGES_H
#include <iostream>
#include <player_avatar_generated.h>
#include <text_message_generated.h>
#include "messages.h"

namespace flat_engine::network {
    class PlayerAvatarMessage final : public IMessage {
        const Game::PlayerAvatarT* player_;
    public:
        explicit PlayerAvatarMessage(std::vector<uint8_t>&& data) : IMessage(std::move(data)) {
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

        static IMessage* create(std::vector<uint8_t>&& data) {
            return new PlayerAvatarMessage(std::move(data));
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

    class TextMessage final : public IMessage {
        const Game::TextMessage* message_;
    public:
        explicit TextMessage(std::vector<uint8_t>&& data) : IMessage(std::move(data)) {
            // todo: логика преобразования сырого буфера в полезные данные
            message_ = nullptr;
            if (auto verifier = flatbuffers::Verifier(data_.data(), data_.size());
                !Game::VerifyTextMessageBuffer(verifier)) {
                is_verified = false;
                message_ = nullptr;
                return;
                }

            message_ = Game::GetTextMessage(data_.data());
            is_verified = message_ != nullptr;
        }

        static IMessage* create(std::vector<uint8_t>&& data) {
            return new TextMessage(std::move(data));
        }

        void process() override {
            std::cout << "received message: " << message_->text()->c_str() << std::endl;
        }
    };
}

#endif //GAME_MESSAGES_H
