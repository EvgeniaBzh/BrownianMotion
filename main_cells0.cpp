#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include "Cells0.h"
#include "Particle.h"

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " N K p [durationSec]\n";
        return 1;
    }

    int N = std::stoi(argv[1]);
    int K = std::stoi(argv[2]);
    double p = std::stod(argv[3]);
    int duration = (argc >= 5) ? std::stoi(argv[4]) : 60;

    std::cout << "=== CELLS0 (No locking) ===\n";
    std::cout << "Parameters: N=" << N << ", K=" << K << ", p=" << p 
              << ", duration=" << duration << "s\n\n";

    Cells0 cells(N, K);  // ТІЛЬКИ Cells0!
    std::atomic<bool> running(true);
    std::atomic<long long> totalMoves(0);
    std::vector<std::thread> threads;

    auto start = std::chrono::high_resolution_clock::now();

    // Запускаємо потоки тільки з Cells0
    for (int i = 0; i < K; i++) {
        threads.emplace_back(particleThreadWithCounter<Cells0>, 
                           std::ref(cells), p, std::ref(running), i, std::ref(totalMoves));
    }

    for (int t = 0; t < duration; t++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "t=" << t << ": ";
        for (int i = 0; i < N; i++) {
            std::cout << cells.getCell(i) << " ";
        }
        std::cout << "\n";
    }

    running = false;
    for (auto &th : threads) th.join();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "\n=== PERFORMANCE RESULTS ===\n";
    std::cout << "Final total: " << cells.total() << " (should be " << K << ")\n";
    std::cout << "Total steps: " << totalMoves.load() << "\n";
    std::cout << "Throughput: " << (totalMoves.load() / elapsed.count()) << " steps/sec\n";

    return 0;
}