#pragma once

#include <server/server_engine.hpp>
#include <iostream>

class GameServer : public ServerEngine {
public:
    explicit GameServer(uint16_t tcp_port, uint16_t udp_port);
    ~GameServer() override = default;

};
