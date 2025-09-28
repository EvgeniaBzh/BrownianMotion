#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include "Cells2.h"
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

    std::cout << "=== CELLS2 (Fine-grained locking) ===\n";
    std::cout << "Parameters: N=" << N << ", K=" << K << ", p=" << p
              << ", duration=" << duration << "s\n\n";

    Cells2 cells(N, K);
    std::atomic<bool> running(true);
    std::atomic<long long> externalCounter(0); // для порівняння (не обов'язково)
    std::vector<std::thread> threads;
    threads.reserve(K);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < K; ++i) {
        threads.emplace_back(particleThreadWithCounter<Cells2>,
                            std::ref(cells), p, std::ref(running), i, std::ref(externalCounter));
    }

    for (int t = 1; t <= duration; ++t) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Беремо snapshot один раз та виводимо — це зменшує кількість блокувань
        auto snap = cells.snapshot();
        std::cout << "t=" << t << ": ";
        for (int i = 0; i < N; ++i) {
            std::cout << snap[i] << (i+1==N ? "" : " ");
        }
        std::cout << "\n";
    }

    running.store(false);
    for (auto &th : threads) {
        if (th.joinable()) th.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "\n=== PERFORMANCE RESULTS ===\n";
    std::cout << "Final total: " << cells.total() << " (should be " << K << ")\n";
    std::cout << "Total steps: " << cells.getTotalMoves() << "\n";
    std::cout << "Throughput: " << (cells.getTotalMoves() / elapsed.count()) << " steps/sec\n";

    return 0;
}
