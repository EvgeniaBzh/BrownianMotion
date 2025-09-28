#include "Cells0.h"

Cells0::Cells0(int n, int k) : cells(n, 0), totalMoves(0) {
    if (n > 0) cells[0] = k;  // усі атоми зліва
}

void Cells0::move(int from, int to) {
    if (from == to) return;
    cells[from]--;
    cells[to]++;
    totalMoves++; // Лічимо кожен рух
}

int Cells0::getCell(int i) const {
    return cells[i];
}

int Cells0::total() const {
    int sum = 0;
    for (int c : cells) sum += c;
    return sum;
}

long long Cells0::getTotalMoves() const {
    return totalMoves.load();
}

void Cells0::resetMoves() {
    totalMoves = 0;
}
