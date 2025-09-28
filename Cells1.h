#ifndef CELLS1_H
#define CELLS1_H

#include <vector>
#include <mutex>
#include <atomic>

class Cells1 {
private:
    std::vector<int> cells;
    mutable std::mutex mtx;
    std::atomic<long long> totalMoves; // лічильник переміщень

public:
    Cells1(int n, int k);

    void move(int from, int to);  // із синхронізацією
    int getCell(int i) const;
    int size() const { return (int)cells.size(); }
    int total() const;

    long long getTotalMoves() const;
    void resetMoves();
};

#endif
