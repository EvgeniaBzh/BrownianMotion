#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include <mutex>
#include <atomic>

class Particle {
public:
    Particle(int id, int pos, int N, double p, std::vector<int>& cells, std::mutex* mtx = nullptr);
    void run();
    void stop();

private:
    int id;
    int position;
    int N;
    double p;
    std::vector<int>& cells;
    std::mutex* mtx;
    std::atomic<bool> running;
};

#endif
