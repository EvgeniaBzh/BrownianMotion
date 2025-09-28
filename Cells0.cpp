#include "Cells0.h"

// створює масив клітинок розміром n
// усі атоми на початку поміщаються у першу клітинку (індекс 0)
Cells0::Cells0(int n, int k) : cells(n, 0), totalMoves(0) {
    if (n > 0) cells[0] = k;
}

// метод переміщення атома з клітинки from у клітинку to
// зменшує кількість у from і збільшує у to
// рахує кількість здійснених переміщень
void Cells0::move(int from, int to) {
    if (from == to) return; // якщо клітинки однакові, нічого не робимо
    cells[from]--;
    cells[to]++;
    totalMoves++;
}

// повертає кількість атомів у клітинці з індексом i
int Cells0::getCell(int i) const {
    return cells[i];
}

// обчислює загальну кількість атомів у всіх клітинках
int Cells0::total() const {
    int sum = 0;
    for (int c : cells) sum += c;
    return sum;
}

// повертає кількість здійснених переміщень
long long Cells0::getTotalMoves() const {
    return totalMoves.load();
}

// скидає лічильник переміщень
void Cells0::resetMoves() {
    totalMoves = 0;
}
