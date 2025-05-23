//
// Created by curo on 23.05.2025.
//

#ifndef INTCLOCK_H
#define INTCLOCK_H
#include <chrono>
#include <cstdint>
#include <ctime>

inline int64_t getMicrosecondsSince2025() {
    using namespace std::chrono;

    // Текущий момент времени
    auto now = system_clock::now();

    // Структура tm для 01.01.2025 00:00:00 UTC
    std::tm tm2025 = {};
    tm2025.tm_year = 2025 - 1900;  // годы с 1900
    tm2025.tm_mon  = 0;            // январь = 0
    tm2025.tm_mday = 1;            // первое число
    tm2025.tm_hour = 0;
    tm2025.tm_min  = 0;
    tm2025.tm_sec  = 0;

    // Преобразуем tm в time_t (UTC)
#ifdef _WIN32
    time_t t2025 = _mkgmtime(&tm2025);
#else
    time_t t2025 = timegm(&tm2025);
#endif

    // Время старта в виде time_point
    auto tp2025 = system_clock::from_time_t(t2025);

    // Разница в микросекундах
    return duration_cast<microseconds>(now - tp2025).count();
}
#endif //INTCLOCK_H
