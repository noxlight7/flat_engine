#include "game_server/game_server.hpp"
#include <iostream>

#include "routes.hpp"
#include "shared_game_deserializer.hpp"
#include "network/server_game_deserializer.hpp"

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
#endif // _WIN32

    auto server = GameServer(flat_engine::network::k_host_tcp_port, flat_engine::network::k_host_udp_port);
    flat_engine::network::GameDeserializeSharedRoutes::initRoutes();
    flat_engine::network::GameDeserializeServerRoutes::initRoutes();
    server.init();
    return 0;
}
