#include "Cells1.h"

// створює масив клітинок розміром n
// усі атоми на початку поміщаються у першу клітинку
Cells1::Cells1(int n, int k) : cells(n, 0), totalMoves(0) {
    if (n > 0) cells[0] = k;
}

// метод переміщення атома з клітинки from у клітинку to
// операція виконується під глобальним мьютексом, що гарантує безпечний доступ при багатопоточності
void Cells1::move(int from, int to) {
    std::lock_guard<std::mutex> lock(mtx); // блокуємо увесь масив
    if (from == to) return; // якщо індекси однакові, нічого не робимо
    cells[from]--;
    cells[to]++;
    totalMoves++;
}

// повертає кількість атомів у клітинці з індексом i
// доступ до масиву захищений м'ютексом
int Cells1::getCell(int i) const {
    std::lock_guard<std::mutex> lock(mtx);
    return cells[i];
}

// обчислює загальну кількість атомів у всіх клітинках
// увесь масив блокується під час підрахунку
int Cells1::total() const {
    std::lock_guard<std::mutex> lock(mtx);
    int sum = 0;
    for (int c : cells) sum += c;
    return sum;
}

// повертає загальну кількість здійснених переміщень
long long Cells1::getTotalMoves() const {
    return totalMoves.load();
}

// скидає лічильник переміщень
void Cells1::resetMoves() {
    totalMoves = 0;
}
