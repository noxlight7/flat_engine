#include "game_server.hpp"

#include "session_controller.hpp"

GameServer::GameServer(const uint16_t tcp_port, uint16_t udp_port)
    : ServerEngine(tcp_port, udp_port, std::make_unique<SessionController>()) {
}


