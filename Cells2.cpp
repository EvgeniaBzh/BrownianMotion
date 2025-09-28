#include "Cells2.h"
#include <algorithm>
#include <memory>

// створює масив клітинок розміром n
// окремий мьютекс для кожної клітинки та встановлює всі атоми у першу клітинку
Cells2::Cells2(int n, int k) : cells(n, 0), cellMutexes(n), totalMoves(0) {
    if (n > 0) cells[0] = k;
}

// метод переміщення атома з клітинки from у клітинку to
// блокує тільки дві клітинки, що дозволяє досягти високої паралельності
void Cells2::move(int from, int to) {
    if (from == to) return;  // якщо клітинки однакові, нічого не робимо

    // блокуємо клітинки у стабільному порядку, щоб уникнути deadlock
    int a = std::min(from, to);
    int b = std::max(from, to);

    std::lock_guard<std::mutex> lockA(cellMutexes[a]);
    std::lock_guard<std::mutex> lockB(cellMutexes[b]);

    // перевірка: чи є атом у клітинці from
    if (cells[from] <= 0) {
        return;  // якщо немає, то рух не виконуємо
    }

    cells[from]--;
    cells[to]++;

    totalMoves.fetch_add(1, std::memory_order_relaxed);  // лічильник переміщень
}

// повертає кількість атомів у клітинці з індексом i
// блокується тільки потрібна клітинка
int Cells2::getCell(int i) const {
    std::lock_guard<std::mutex> lock(cellMutexes[i]);
    return cells[i];
}

// повертає кількість клітинок у кристалі
int Cells2::size() const {
    return static_cast<int>(cells.size());
}

// обчислює загальну кількість атомів у всіх клітинках
// для коректності блокує всі клітинки у стабільному порядку
int Cells2::total() const {
    std::vector<std::unique_lock<std::mutex>> locks;
    locks.reserve(cellMutexes.size());
    for (auto &mtx : cellMutexes) {
        locks.emplace_back(mtx);
    }

    int sum = 0;
    for (int v : cells) sum += v;
    return sum;
}

// повертає знімок поточного стану всіх клітинок
// для цього блокує всі клітинки та копіює вектор
std::vector<int> Cells2::snapshot() const {
    std::vector<std::unique_lock<std::mutex>> locks;
    locks.reserve(cellMutexes.size());
    for (auto &mtx : cellMutexes) {
        locks.emplace_back(mtx);
    }
    return cells;
}

// повертає загальну кількість переміщень
long long Cells2::getTotalMoves() const {
    return totalMoves.load(std::memory_order_relaxed);
}

// скидає лічильник переміщень
void Cells2::resetMoves() {
    totalMoves.store(0, std::memory_order_relaxed);
}
