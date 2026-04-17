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
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
        return 1;
    }
    Instance inst;
    if(argc == 3){
        inst = Instance::fromFile(argv[1], std::atoi(argv[2]));
    }
    else{
        inst = Instance::fromFile(argv[1]);
    }
    if (inst.size() == 0){
        return 1;
    }

    std::vector<Algorithm> algos = {
        {"Sort by R", sortByR},
        {"Sort by D", sortByD},
        {"Schrage", schrage},
        {"Vegas", vegasSort}
    };

    if (inst.size() <= 12) {
        algos.push_back({"Brute Force", bruteForce});
    }

    //Formatowanie napisu (tutaj pomógł Gemini)
    std::cout << std::left
              << std::setw(22) << "Algorithm"
              << std::setw(8)  << "Lmax"
              << std::setw(14) << "Time [us]" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    for (const auto& algo : algos) {
        auto start = std::chrono::high_resolution_clock::now();
        Solution sol = algo.func(inst);
        auto end = std::chrono::high_resolution_clock::now();
        double us = std::chrono::duration<double, std::micro>(end - start).count();

        std::cout << std::left
                  << std::setw(22) << algo.name
                  << std::setw(8)  << sol.lmax
                  << std::setw(14) << std::fixed << std::setprecision(1) << us;

        /*for (const auto& j : sol.schedule) {
            std::cout << j.id << " ";
        }*/
        std::cout << std::endl;
    }

    return 0;
}