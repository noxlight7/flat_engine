#pragma once

#include <server/server_engine.hpp>
#include <iostream>

class GameServer : public ServerEngine {
public:
    explicit GameServer(uint16_t port);
    ~GameServer() override = default;

};
