#include "Cells2.h"
#include <algorithm>
#include <memory>

Cells2::Cells2(int n, int k) : cells(n, 0), cellMutexes(n), totalMoves(0) {
    if (n > 0) cells[0] = k;
}

void Cells2::move(int from, int to) {
    if (from == to) return;

    // Завжди блокувати в стабільному порядку індексів, щоб уникнути deadlock
    int a = std::min(from, to);
    int b = std::max(from, to);

    std::lock_guard<std::mutex> lockA(cellMutexes[a]);
    std::lock_guard<std::mutex> lockB(cellMutexes[b]);

    // Додаткова перевірка — можливо значення вже змінилось (але все одно коректно)
    if (cells[from] <= 0) {
        // нічого не переміщаємо якщо немає частинки (безпечна перевірка)
        return;
    }

    cells[from]--;
    cells[to]++;

    totalMoves.fetch_add(1, std::memory_order_relaxed);
}

int Cells2::getCell(int i) const {
    // Використовується лише у випадках, коли потрібен окремий доступ до однієї клітинки.
    std::lock_guard<std::mutex> lock(cellMutexes[i]);
    return cells[i];
}

int Cells2::size() const {
    return static_cast<int>(cells.size());
}

int Cells2::total() const {
    // Заблокуємо всі mutex'и в зростаючому порядку, щоб отримати консистентну суму
    std::vector<std::unique_lock<std::mutex>> locks;
    locks.reserve(cellMutexes.size());
    for (auto &mtx : cellMutexes) {
        locks.emplace_back(mtx);
    }

    int sum = 0;
    for (int v : cells) sum += v;
    return sum;
}

std::vector<int> Cells2::snapshot() const {
    std::vector<std::unique_lock<std::mutex>> locks;
    locks.reserve(cellMutexes.size());
    for (auto &mtx : cellMutexes) {
        locks.emplace_back(mtx);
    }
    // Копіюємо масив під блокуванням
    return cells;
}

long long Cells2::getTotalMoves() const {
    return totalMoves.load(std::memory_order_relaxed);
}

void Cells2::resetMoves() {
    totalMoves.store(0, std::memory_order_relaxed);
}
