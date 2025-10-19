// compiler - MSVC
// Variant 10

#include <cmath>
#include <chrono>
#include <execution>
#include <format>
#include <iostream>
#include <latch>
#include <random>
#include <thread>
#include <vector>

bool Predicate(const int x) {
    return x * x + x - cos(x) > 0;
}

std::vector<int> RandomSeq(const std::size_t size, std::mt19937& mt_engine) {
    std::vector<int> rand_seq;
    rand_seq.reserve(size);

    std::uniform_int_distribution<std::mt19937::result_type> distribution(0, 100);

    for (int i = 0; i < size; i++) rand_seq.push_back(distribution(mt_engine));

    return rand_seq;
}

void Part1(const std::vector<std::vector<int>>& sequences) {
    std::cout << "No policy (sequential):\n";

    for (const auto& seq : sequences) {
        auto start = std::chrono::high_resolution_clock::now();
        std::for_each(seq.begin(), seq.end(), Predicate);
        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "Sequence size = " << seq.size()
            << "; time = "
            << std::chrono::duration<double, std::milli>(end - start).count()
            << " ms\n";
    }
}

void Part2(const std::vector<std::vector<int>>& sequences) {

    auto f = [](int x) { return x * x + x - std::cos(x) > 0;};

    for (const auto& seq : sequences) {
        std::cout << std::format("\nSequence size = {}\n", seq.size());

        auto start_seq = std::chrono::high_resolution_clock::now();
        std::for_each(std::execution::seq, seq.begin(), seq.end(), f);
        auto end_seq = std::chrono::high_resolution_clock::now();

        double t_seq = std::chrono::duration<double, std::milli>(end_seq - start_seq).count();
        std::cout << std::format("  Sequential (seq): {:.4f} ms\n", t_seq);

        auto start_par = std::chrono::high_resolution_clock::now();
        std::for_each(std::execution::par, seq.begin(), seq.end(), f);
        auto end_par = std::chrono::high_resolution_clock::now();

        double t_par = std::chrono::duration<double, std::milli>(end_par - start_par).count();
        std::cout << std::format("  Parallel (par): {:.4f} ms\n", t_par);

        auto start_unseq = std::chrono::high_resolution_clock::now();
        std::for_each(std::execution::unseq, seq.begin(), seq.end(), f);
        auto end_unseq = std::chrono::high_resolution_clock::now();

        double t_unseq = std::chrono::duration<double, std::milli>(end_unseq - start_unseq).count();
        std::cout << std::format("  Unsequenced (unseq): {:.4f} ms\n", t_unseq);

        auto start_par_unseq = std::chrono::high_resolution_clock::now();
        std::for_each(std::execution::par_unseq, seq.begin(), seq.end(), f);
        auto end_par_unseq = std::chrono::high_resolution_clock::now();

        double t_par_unseq = std::chrono::duration<double, std::milli>(end_par_unseq - start_par_unseq).count();


        std::cout << std::format("  Parallel Unsequenced (par_unseq): {:.4f} ms\n", t_par_unseq);
    }
}


void Part3(const std::vector<std::vector<int>>& sequences) {
    std::cout << "\nMultithreaded for_each (variable K):\n";
    std::cout << std::format("{:<8} {:<15} {:<15}\n", "K", "Seq size", "Time(ms)");
    std::cout << std::string(40, '-') << "\n";

    int best_K = 0;
    double best_avg_time = std::numeric_limits<double>::max();

    for (int K = 2; K <= 16; K += 2) {
        double total_time = 0.0;

        for (const auto& seq : sequences) {
            std::vector<std::vector<int>> chunks(K);
            int chunk_size = seq.size() / K;
            int remainder = seq.size() % K;
            int start = 0;
            for (int i = 0; i < K; ++i) {
                int end = start + chunk_size + (i < remainder ? 1 : 0);
                chunks[i] = std::vector<int>(seq.begin() + start, seq.begin() + end);
                start = end;
            }

            std::vector<std::jthread> threads;
            std::latch done(K);
            auto start_time = std::chrono::high_resolution_clock::now();

            for (auto& chunk : chunks) {
                threads.emplace_back([chunk, &done]() mutable {
                    std::for_each(chunk.begin(), chunk.end(), [](int x) { Predicate(x); });
                    done.count_down();
                    });
            }

            done.wait();
            auto end_time = std::chrono::high_resolution_clock::now();
            double elapsed_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
            total_time += elapsed_ms;

            std::cout << std::format("{:<8} {:<15} {:<15.4f}\n", K, seq.size(), elapsed_ms);
        }

        double avg_time = total_time / sequences.size();
        if (avg_time < best_avg_time) {
            best_avg_time = avg_time;
            best_K = K;
        }
    }

    std::cout << "-----------------------------------\n";
    std::cout << std::format("Best K = {} (avg time = {:.4f} ms)\n", best_K, best_avg_time);
    std::cout << std::format("Hardware : {}\n", std::thread::hardware_concurrency());
}



int main() {
    std::random_device rd;
    std::mt19937 mt_engine(rd());

    std::vector<std::vector<int>> sequences;

    int starting_size = 100;

    for (int i = 0; i < 5; ++i) {
        sequences.push_back(RandomSeq(starting_size, mt_engine));
        starting_size *= 10;
    }

    Part1(sequences);
    Part2(sequences);
    Part3(sequences);

    return 0;
}
