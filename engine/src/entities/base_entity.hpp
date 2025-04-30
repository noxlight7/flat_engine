#pragma once
#include <cstdint>

#include "utils/pool_id.hpp"

class BaseEntity {
    uint32_t m_id;
    PoolID *m_pool_id;

public:
    explicit BaseEntity(uint32_t id);
    explicit BaseEntity(PoolID *pool_id);
    BaseEntity(BaseEntity &&entity) noexcept ;
    virtual ~BaseEntity();
    [[nodiscard]] uint32_t getID() const { return m_id; }
};
