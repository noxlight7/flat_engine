//
// Created by curo on 15.03.2025.
//

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <boost/asio.hpp>
#include <iostream>
#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>
#include <cstdint>

class Scheduler {
public:
    using EventId = uint64_t;

    explicit Scheduler(boost::asio::io_context& ioContext)
        : io_context_(ioContext), next_event_id_(1)
    {}

    ~Scheduler() {
        cancelAllEvents();
    }

    // Планирование одноразового события через заданный интервал времени.
    // Возвращает уникальный идентификатор события.
    template <typename Duration, typename Handler>
    EventId scheduleAfter(Duration duration, Handler handler) {
        EventId id = next_event_id_++;
        auto timer = std::make_shared<boost::asio::steady_timer>(io_context_, duration);
        auto scheduledEvent = std::make_shared<ScheduledEvent>();
        scheduledEvent->timer = timer;
        scheduledEvent->periodic = false;
        events_.emplace(id, scheduledEvent);

        timer->async_wait([this, id, timer, handler](const boost::system::error_code& ec) {
            if (!ec) {
                handler();
            }
            events_.erase(id);
        });
        return id;
    }

    // Планирование одноразового события на конкретный момент времени.
    template <typename Handler>
    EventId scheduleAt(const std::chrono::steady_clock::time_point& timePoint, Handler handler) {
        EventId id = next_event_id_++;
        auto timer = std::make_shared<boost::asio::steady_timer>(io_context_, timePoint);
        auto scheduledEvent = std::make_shared<ScheduledEvent>();
        scheduledEvent->timer = timer;
        scheduledEvent->periodic = false;
        events_.emplace(id, scheduledEvent);

        timer->async_wait([this, id, timer, handler](const boost::system::error_code& ec) {
            if (!ec) {
                handler();
            }
            events_.erase(id);
        });
        return id;
    }

    // Планирование периодического события, которое срабатывает каждые dt.
    // Возвращает идентификатор события.
    template <typename Duration, typename Handler>
    EventId schedulePeriodic(Duration dt, Handler handler) {
        EventId id = next_event_id_++;
        auto timer = std::make_shared<boost::asio::steady_timer>(io_context_, dt);
        auto scheduledEvent = std::make_shared<ScheduledEvent>();
        scheduledEvent->timer = timer;
        scheduledEvent->periodic = true;
        scheduledEvent->period = dt;
        events_.emplace(id, scheduledEvent);

        auto callback = std::make_shared<std::function<void(const boost::system::error_code&)>>();
        *callback = [this, id, dt, handler, timer, callback](const boost::system::error_code& ec) {
            if (events_.find(id) == events_.end())
                return;
            if (!ec) {
                handler();
                timer->expires_after(dt);
                timer->async_wait(*callback);
            } else {
                events_.erase(id);
            }
        };
        timer->async_wait(*callback);
        return id;
    }

    // Отмена события по его идентификатору.
    void cancelEvent(EventId id) {
        auto it = events_.find(id);
        if (it != events_.end()) {
            it->second->timer->cancel();
            events_.erase(it);
        }
    }

    // Отмена всех запланированных событий.
    void cancelAllEvents() {
        for (auto& pair : events_) {
            pair.second->timer->cancel();
        }
        events_.clear();
    }

    // Отмена только периодических событий.
    void cancelAllPeriodicEvents() {
        for (auto it = events_.begin(); it != events_.end(); ) {
            if (it->second->periodic) {
                it->second->timer->cancel();
                it = events_.erase(it);
            } else {
                ++it;
            }
        }
    }

private:
    struct ScheduledEvent {
        std::shared_ptr<boost::asio::steady_timer> timer;
        bool periodic;
        std::chrono::steady_clock::duration period;
    };

    boost::asio::io_context& io_context_;
    EventId next_event_id_;
    std::unordered_map<EventId, std::shared_ptr<ScheduledEvent>> events_;
};

#endif //SCHEDULER_H
