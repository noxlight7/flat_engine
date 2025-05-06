//
// Created by curo on 25.03.2025.
//

#ifndef SESSION_H
#define SESSION_H
#include <memory>
#include <boost/asio.hpp>

namespace flat_engine::network {
    struct  IGameData {
        virtual ~IGameData() = default;
    };

    class ISession {
        std::unique_ptr<IGameData> game_data_;
        uint32_t session_id_;
        boost::asio::strand<boost::asio::any_io_executor> strand_;
        std::atomic<bool> is_deleted_;
    public:
        virtual ~ISession() = default;
        ISession() = delete;
        explicit ISession(
            boost::asio::io_context& io_context,
            std::unique_ptr<IGameData> game_data,
            uint32_t session_id)
            : game_data_(std::move(game_data)),
              session_id_(session_id),
              strand_(make_strand(io_context)),
              is_deleted_(false) {}

        [[nodiscard]] IGameData *getGameData() const { return game_data_.get(); }

        [[nodiscard]] uint32_t getSessionID() const { return session_id_; }

        boost::asio::strand<boost::asio::any_io_executor>& getStrand() {
            return strand_;
        }

        [[nodiscard]] bool isDeleted() const { return is_deleted_.load(); }
        void setDeleted() { is_deleted_ = true; }
    };
}

#endif //SESSION_H
