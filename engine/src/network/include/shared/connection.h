//
// Created by curo on 20.01.2025.
//

#ifndef CONNECTION_H
#define CONNECTION_H
#include "transport.h"
namespace flat_engine::network {
    class Connection {
        std::unique_ptr<ITransport> transport_;
    public:
        explicit Connection(std::unique_ptr<ITransport> transport);
    };
}
#endif //CONNECTION_H
