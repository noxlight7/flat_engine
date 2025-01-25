//
// Created by curo on 13.01.2025.
//

#ifndef MESSAGES_H
#define MESSAGES_H
namespace flat_engine::network {
    class IMessage {
    protected:
        bool is_verified = false;
        std::vector<uint8_t> data_;
        explicit IMessage(std::vector<uint8_t>&& data) : data_(std::move(data)) {}
    public:
        IMessage() = delete;
        IMessage(const IMessage&) = delete;
        IMessage& operator=(const IMessage&) = delete;
        IMessage(IMessage&&) = default;
        IMessage& operator=(IMessage&&) = default;
        [[nodiscard]] bool isVerified() const { return is_verified; }
        // вызывается на фазе обработки сообщений
        virtual void process() = 0;
        virtual ~IMessage() = default;
    };
}

#endif //MESSAGES_H
