//
// Created by curo on 28.04.2025.
//

#ifndef TEXT_MESSAGE_H
#define TEXT_MESSAGE_H
#include <iostream>
#include <player_avatar_generated.h>
#include <text_message_generated.h>
#include "messages.h"

namespace flat_engine::network {
    class TextMessage final : public IMessage {
        const Game::TextMessage* message_;
    public:
        explicit TextMessage(std::vector<uint8_t>&& data, std::shared_ptr<ISession> session)
            : IMessage(std::move(data), std::move(session)) {
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

        static IMessage* create(std::vector<uint8_t>&& data, std::shared_ptr<ISession> session) {
            return new TextMessage(std::move(data), std::move(session));
        }

        void process() override {
            std::cout << "received message: " << message_->text()->c_str() << std::endl;
        }
    };
}

#endif //TEXT_MESSAGE_H
