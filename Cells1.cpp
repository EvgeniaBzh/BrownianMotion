#include "Cells1.h"

Cells1::Cells1(int n, int k) : cells(n, 0), totalMoves(0) {
    if (n > 0) cells[0] = k;
}

void Cells1::move(int from, int to) {
    std::lock_guard<std::mutex> lock(mtx);
    if (from == to) return;
    cells[from]--;
    cells[to]++;
    totalMoves++; // підрахунок переміщень
}

int Cells1::getCell(int i) const {
    std::lock_guard<std::mutex> lock(mtx);
    return cells[i];
}

int Cells1::total() const {
    std::lock_guard<std::mutex> lock(mtx);
    int sum = 0;
    for (int c : cells) sum += c;
    return sum;
}

long long Cells1::getTotalMoves() const {
    return totalMoves.load();
}

void Cells1::resetMoves() {
    totalMoves = 0;
}
