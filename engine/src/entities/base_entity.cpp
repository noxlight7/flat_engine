#include "base_entity.hpp"

BaseEntity::BaseEntity(const uint32_t id) : m_id(id), m_pool_id(nullptr) {
}

BaseEntity::BaseEntity(PoolID* pool_id) {
    m_pool_id = pool_id;
    m_id = m_pool_id->genID();
}

BaseEntity::BaseEntity(BaseEntity&& entity)  noexcept {
    m_id = entity.m_id;
    m_pool_id = entity.m_pool_id;
    entity.m_id = 0;
    entity.m_pool_id = nullptr;
}

BaseEntity::~BaseEntity() {
    if (m_pool_id != nullptr) {
        m_pool_id->releaseID(m_id);
    }
}
