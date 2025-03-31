#include "game_server.hpp"

#include "session_controller.hpp"

GameServer::GameServer(const uint16_t port)
    : ServerEngine(port, std::make_unique<SessionController>()) {
}


