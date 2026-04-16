#include "Algorithms.h"
#include <iostream>
#include <chrono>
#include <functional>
#include <iomanip>
#include <climits>

struct Algorithm {
    std::string name;
    std::function<Solution(const Instance&)> func;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file> [UB]" << std::endl;
        return 1;
    }

    Instance inst = Instance::fromFile(argv[1]);
    if (inst.size() == 0) return 1;

    inst.print();
    std::cout << std::endl;

    int knownUB = (argc >= 3) ? std::stoi(argv[2]) : INT_MIN;

    std::vector<Algorithm> algos = {
        {"Sort by R",          sortByR},
        {"Sort by D (EDD)",    sortByD},
        {"Schrage",            schrage},
    };

    if (inst.size() <= 12) {
        algos.insert(algos.begin() + 2, {"Brute Force", bruteForce});
    }

    std::cout << std::left
              << std::setw(22) << "Algorithm"
              << std::setw(8)  << "Lmax"
              << std::setw(14) << "Time [us]"
              << "Schedule" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    for (const auto& algo : algos) {
        auto start = std::chrono::high_resolution_clock::now();
        Solution sol = algo.func(inst);
        auto end = std::chrono::high_resolution_clock::now();
        double us = std::chrono::duration<double, std::micro>(end - start).count();

        std::cout << std::left
                  << std::setw(22) << algo.name
                  << std::setw(8)  << sol.lmax
                  << std::setw(14) << std::fixed << std::setprecision(1) << us;

        for (const auto& j : sol.schedule) {
            std::cout << j.id << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}