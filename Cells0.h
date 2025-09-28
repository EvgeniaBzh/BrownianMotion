#ifndef CELLS0_H
#define CELLS0_H

#include <vector>
#include <atomic>

class Cells0 {
private:
    std::vector<int> cells;
    std::atomic<long long> totalMoves;

public:
    Cells0(int n, int k);

    void move(int from, int to);
    int getCell(int i) const;
    int size() const { return (int)cells.size(); }
    int total() const;

    long long getTotalMoves() const;
    void resetMoves();
};

#endif
