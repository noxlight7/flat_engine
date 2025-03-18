#include "base_entity.hpp"

BaseEntity::BaseEntity(const uint32_t id) : m_id(id), m_pool_id(nullptr) {
}

BaseEntity::BaseEntity(PoolID* pool_id) {
    m_pool_id = pool_id;
    m_id = m_pool_id->genID();
}

BaseEntity::~BaseEntity() {
    if (m_pool_id != nullptr) {
        m_pool_id->releaseID(m_id);
    }
}
