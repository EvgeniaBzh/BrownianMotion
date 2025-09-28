#ifndef PARTICLE_H
#define PARTICLE_H

#include <random>
#include <atomic>
#include <thread>
#include <chrono>

// базова функція потоку, моделює рух однієї частинки по клітинках
template <typename Cells>
void particleThread(Cells &cells, double p, std::atomic<bool> &running, int id) {
    int pos = 0; // початкова позиція частинки в масиві клітинок
    // гарантує, що генератор випадкових чисел у кожного потоку свій
    thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0); // рівномірний розподіл [0,1]

    while (running.load(std::memory_order_relaxed)) { // цикл руху частинки
        double m = dist(gen); // випадкове число для визначення напрямку
        int newPos = (m > p) ? pos + 1 : pos - 1;

        // обмеження межами масиву: якщо нова позиція виходить за межі, залишаємося на місці
        if (newPos < 0 || newPos >= cells.size()) {
            newPos = pos;
        }

        cells.move(pos, newPos); // переміщення частинки у масиві клітинок
        pos = newPos; // оновлення поточної позиції

        // коротка пауза, щоб не перевантажувати CPU (мінімізує busy-wait)
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
}

// версія потоку з підрахунком кроків
// використовується зовнішній лічильник externalCounter
template <typename Cells>
void particleThreadWithCounter(Cells &cells, double p, std::atomic<bool> &running, 
                               int id, std::atomic<long long> &externalCounter) {
    int pos = 0; // початкова позиція
    thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    long long localMoves = 0; // локальний лічильник кроків для цього потоку

    while (running.load(std::memory_order_relaxed)) {
        double m = dist(gen);
        int newPos = (m > p) ? pos + 1 : pos - 1;

        // обмеження межами масиву
        if (newPos < 0 || newPos >= cells.size()) {
            newPos = pos;
        }

        cells.move(pos, newPos); // рух частинки
        pos = newPos;
        localMoves++; // збільшуємо локальний лічильник

        // щоб зменшити конкуренцію при оновленні глобального лічильника, не додаємо кожен крок відразу, а накопичуємо 100 кроків
        if (localMoves >= 100) {
            externalCounter.fetch_add(localMoves, std::memory_order_relaxed);
            localMoves = 0; // обнуляємо локальний лічильник
        }

        // коротка пауза для плавності симуляції
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }

    // після завершення потоку додаємо залишок кроків, які ще не були підраховані
    if (localMoves > 0) {
        externalCounter.fetch_add(localMoves, std::memory_order_relaxed);
    }
}

#endif // PARTICLE_H
