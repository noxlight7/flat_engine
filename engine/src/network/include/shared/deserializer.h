//
// Created by curo on 10.01.2025.
//

#ifndef DESERIALIZER_H
#define DESERIALIZER_H
#include <functional>
#include <utility>
#include <vector>
#include <flatbuffers/flatbuffers.h>
#include <boost/lockfree/queue.hpp>
#include <network_settings.h>

#include "messages.h"

namespace flat_engine::network {
    using MessageHandler = std::function<IMessage*(
        std::vector<uint8_t> &&buffer, std::shared_ptr<ISession> session)>;

    constexpr uint32_t k_max_messages_in_queue = 100000;

    enum MessageAddResult: uint8_t {
        k_success = 0,
        k_invalid_route = 1,
        k_invalid_message = 2,
        k_full_queue = 3,
    };

    class Deserializer {
        // static constexpr MessageRouteType max_route_id = 0x1fff;
        std::unordered_map<MessageRouteType, MessageHandler> handlers;
        boost::lockfree::queue<IMessage*> messagesQueue;

        Deserializer(): messagesQueue(k_max_messages_in_queue) {}

    public:
        Deserializer(const Deserializer&) = delete;
        Deserializer& operator=(const Deserializer&) = delete;
        Deserializer(Deserializer&&) = delete;
        Deserializer& operator=(Deserializer&&) = delete;

        MessageAddResult addMessage(const MessageRouteType route_id, std::vector<uint8_t>&& buffer, std::shared_ptr<ISession> session) {
            if (!handlers.contains(route_id)) {
                return k_invalid_route;
            }
            auto message = handlers[route_id](std::move(buffer), std::move(session));
            if (message == nullptr)
                return k_invalid_route;

            if (!message->isVerified()) {
                delete message;
                return k_invalid_message;
            }

            if (!messagesQueue.push(message)) {
                delete message;
                return k_full_queue;
            }

            return k_success;
            // constexpr uint32_t max_attempts = 10;
            // for (uint32_t attempt = 0; attempt < max_attempts; ++attempt) {
            //     if (messagesQueue.push(std::move(message)))
            //         return true;
            //
            //     // здесь нужно подождать одну милисекунду
            // }
            // return false;
        }

        IMessage* getNextMessage() {
            if (IMessage* message = nullptr; messagesQueue.pop(message)) {
                return message;
            }
            return nullptr;
        }

        void processAllMessages(uint32_t num_threads) {
            std::vector<std::thread> threads;
            for (unsigned int i = 0; i < num_threads; i++) {
                threads.emplace_back([this] {
                    auto message = getNextMessage();
                    while (message != nullptr) {
                        message->processSession();
                        message = getNextMessage();
                    }
                });
            }

            for (auto& thread : threads)
                thread.join();
        }

        static Deserializer& instance() {
            static Deserializer deserializer;
            return deserializer;
        }

        void registerHandler(const MessageRouteType route_id, MessageHandler handler) {
            handlers[route_id] = std::move(handler);
        }
    };
}
#endif //DESERIALIZER_H
