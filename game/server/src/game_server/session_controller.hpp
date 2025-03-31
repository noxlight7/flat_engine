//
// Created by curo on 22.03.2025.
//

#ifndef SESSION_CONTROLLER_H
#define SESSION_CONTROLLER_H
#include <session_controller.h>
#include <string>
#include <unordered_map>

struct PlayerData : flat_engine::network::IGameData {
    uint32_t player_id{};
    uint32_t session_id{};
    std::string login;
    PlayerData() = default;
    PlayerData(int player_id, uint32_t session_id, std::string login)
        : player_id(player_id), session_id(session_id), login(std::move(login)) {

    }
};

class SessionController final : public flat_engine::network::ISessionController {
    std::unordered_map<uint32_t, PlayerData> m_session_id_player_data;
    std::unordered_map<uint32_t, uint32_t> m_player_id_session_id;
public:
    ~SessionController() override = default;
    void addSession(const PlayerData& player_data) {
        const uint32_t player_id = player_data.player_id;
        const uint32_t session_id = player_data.session_id;
        m_player_id_session_id[player_id] = session_id;
        m_session_id_player_data[session_id] = player_data;
    }
    void onConnect(uint32_t session_id) override {
        std::cout << "[Game] Connect session id: " << session_id << std::endl;
        addSession(PlayerData{0, session_id, "123"});
    }
    void onDisconnect(const uint32_t session_id) override {
        if (m_session_id_player_data.contains(session_id)) {
            m_player_id_session_id.erase(m_session_id_player_data[session_id].player_id);
            m_session_id_player_data.erase(session_id);
            std::cout << "[Game] Disconnect session id: " << session_id << std::endl;
        }
    }

    std::unique_ptr<flat_engine::network::IGameData> generateStartData() override {
        return std::make_unique<PlayerData>();
    }
};

#endif //SESSION_CONTROLLER_H
