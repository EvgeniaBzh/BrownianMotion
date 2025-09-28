#ifndef PARTICLE_H
#define PARTICLE_H

#include <random>
#include <atomic>
#include <thread>
#include <chrono>

// Базова версія потоку (без підрахунку)
template <typename Cells>
void particleThread(Cells &cells, double p, std::atomic<bool> &running, int id) {
    int pos = 0;
    thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    while (running.load(std::memory_order_relaxed)) {
        double m = dist(gen);
        int newPos = (m > p) ? pos + 1 : pos - 1;

        // Відбивання на межах
        if (newPos < 0 || newPos >= cells.size()) {
            newPos = pos;
        }

        cells.move(pos, newPos);
        pos = newPos;

        // Коротка пауза, щоб уникнути 100% CPU busy-wait
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
}

// Версія з підрахунком (для додаткових метрик)
// Зверни увагу: Cells2 вже має свій внутрішній totalMoves, 
// тому цей лічильник використовується тільки для зовнішнього моніторингу.
template <typename Cells>
void particleThreadWithCounter(Cells &cells, double p, std::atomic<bool> &running, 
                               int id, std::atomic<long long> &externalCounter) {
    int pos = 0;
    thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    long long localMoves = 0;

    while (running.load(std::memory_order_relaxed)) {
        double m = dist(gen);
        int newPos = (m > p) ? pos + 1 : pos - 1;

        // Відбивання
        if (newPos < 0 || newPos >= cells.size()) {
            newPos = pos;
        }

        cells.move(pos, newPos);
        pos = newPos;
        localMoves++;

        // Рідкісне оновлення глобального лічильника (зменшує конкуренцію)
        if (localMoves >= 100) {
            externalCounter.fetch_add(localMoves, std::memory_order_relaxed);
            localMoves = 0;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }

    // Додаємо залишок переходів при виході
    if (localMoves > 0) {
        externalCounter.fetch_add(localMoves, std::memory_order_relaxed);
    }
}

#endif // PARTICLE_H
