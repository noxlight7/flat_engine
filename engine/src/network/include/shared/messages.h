//
// Created by curo on 13.01.2025.
//

#ifndef MESSAGES_H
#define MESSAGES_H
#include <cstdint>
#include <server_session.h>
#include <utility>
#include <vector>

namespace flat_engine::network {
    class IMessage {
    protected:
        std::shared_ptr<ISession> session_;
        bool is_verified = false;
        std::vector<uint8_t> data_;
        explicit IMessage(std::vector<uint8_t>&& data, std::shared_ptr<ISession> session)
            : data_(std::move(data)), session_(std::move(session)) {}


        
    public:
        IMessage() = delete;
        IMessage(const IMessage&) = delete;
        IMessage& operator=(const IMessage&) = delete;
        IMessage(IMessage&&) = default;
        IMessage& operator=(IMessage&&) = default;
        [[nodiscard]] bool isVerified() const { return is_verified; }
        // вызывается на фазе обработки сообщений
        virtual void process() = 0;
        void processSession() {
            boost::asio::post(session_->getStrand(), [this] {
                auto self = std::unique_ptr<IMessage>(this);
                if (!session_->isDeleted())
                    this->process();
            });
        }
        virtual ~IMessage() = default;
    };
}

#endif //MESSAGES_H
