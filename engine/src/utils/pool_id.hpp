//
// Created by curo on 03.03.2025.
//

#ifndef ID_POOL_H
#define ID_POOL_H
#include <deque>
#include <queue>
#include <cstdint>
#include <memory>
#include <iostream>

#include "scheduler.hpp"

inline std::ostream& operator<<(std::ostream& os, const std::queue<uint32_t>& queue) {
    auto out_queue = std::queue(queue);
    while (!out_queue.empty()) {
        os << out_queue.front() << " ";
        out_queue.pop();
    }
    return os;
}

// todo: необходимо добавить:
//  1. Оптимизация по памяти. Хранить свободные id диапазонами
//  (либо просто уменьшать last_id при освобождении m_last_id - 1)
//  2. Оптимизация по скорости. Использовать lockfree очереди
class PoolID {
    mutable std::mutex mutex_;
    Scheduler& m_scheduler;
    Scheduler::EventId m_periodic_shift_id{};
    std::deque<std::unique_ptr<std::queue<uint32_t>>> m_pools;
    uint32_t m_last_id;
    std::queue<uint32_t> m_main_pool;
    std::unique_ptr<std::queue<uint32_t>> m_current_cold_pool;
    int m_cold_pools_amount;
    int m_period;

public:
    PoolID(const int cold_pools_amount, const int period, Scheduler& scheduler)
        : m_scheduler(scheduler),
          m_last_id(1),
          m_current_cold_pool(std::make_unique<std::queue<uint32_t>>()),
          m_cold_pools_amount(cold_pools_amount),
          m_period(period) {

    }

    void init() {
        m_periodic_shift_id = m_scheduler.schedulePeriodic(
            std::chrono::seconds(m_period), [this] {
            this->shiftPool();
        });
    }

    void shiftPool() {
        while (m_pools.size() >= static_cast<size_t>(m_cold_pools_amount)) {
            const auto pool_ptr = std::move(m_pools.front());
            m_pools.pop_front();

            while (!pool_ptr->empty()) {
                m_main_pool.push(pool_ptr->front());
                pool_ptr->pop();
            }
        }
        m_pools.push_back(std::move(m_current_cold_pool));
        m_current_cold_pool = std::make_unique<std::queue<uint32_t>>();
    }

    uint32_t genID() {
        std::lock_guard lock(mutex_);
        if (m_main_pool.empty()) {
            if (m_last_id != UINT32_MAX)
                return m_last_id++;
            else
                return 0;
        } else {
            const auto id = m_main_pool.front();
            m_main_pool.pop();
            return id;
        }
    }

    void releaseID(const uint32_t id) {
        std::lock_guard lock(mutex_);
        m_current_cold_pool->push(id);
    }

    friend std::ostream& operator<<(std::ostream& os, const PoolID& pool) {
        std::lock_guard lock(pool.mutex_);
        os << "main_pool: " << pool.m_main_pool << std::endl;
        os << "current_cold_pool: " << *pool.m_current_cold_pool << std::endl;
        os << "cold_pools_amount: " << pool.m_cold_pools_amount << std::endl;
        os << "period: " << pool.m_period << std::endl;

        int i = 0;

        for (const auto& pool_ptr : pool.m_pools) {
            os << "pool " << i++ << ": " << *pool_ptr << std::endl;
        }
        return os;
    }

    ~PoolID() {
        m_scheduler.cancelEvent(m_periodic_shift_id);
    }
};


#endif //ID_POOL_H
