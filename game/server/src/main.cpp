#include "game_server/game_server.hpp"
#include <iostream>

#include "routes.hpp"

int main(int argc, char* argv[]) {
    auto server = GameServer(flat_engine::network::k_host_tcp_port);
    server.init();
    return 0;
}
