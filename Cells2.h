#ifndef CELLS2_H
#define CELLS2_H

#include <vector>
#include <mutex>
#include <atomic>

class Cells2 {
private:
    std::vector<int> cells;
    mutable std::vector<std::mutex> cellMutexes; // окремий mutex для кожної клітинки
    std::atomic<long long> totalMoves; // лічильник переміщень

public:
    Cells2(int n, int k);

    // Виконати переміщення з from -> to (to може бути == from -> нічого не робимо)
    void move(int from, int to);

    // Повернути копію значення клітинки (без частих індивідуальних lock'ів з main)
    int getCell(int i) const;

    // Повернути розмір
    int size() const;

    // Повернути суму (під блокуванням усіх mutex'ів в одному порядку)
    int total() const;

    // Зробити знімок (копію) всього масиву під блокуванням (без викликів getCell в циклі)
    std::vector<int> snapshot() const;

    long long getTotalMoves() const; // повернути кількість переміщень
    void resetMoves();               // обнулити лічильник
};

#endif // CELLS2_H
