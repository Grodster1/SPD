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

struct Results{
    std::string name;
    int lmax;
    double time_us;
};

std::vector<double> calculateError(const std::vector<int> lmaxs){
    int lmin = *std::min_element(lmaxs.begin(), lmaxs.end());
    std::vector<double> errors;
    for(int i = 0; i < lmaxs.size(); ++i){
        double error = (double)(lmaxs[i] - lmin)/ abs(lmin) * 100;
        errors.push_back(error);
    }
    return errors;
}

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

    std::vector<int> lmaxs;
    std::vector<Results> results;

    std::vector<Algorithm> algos = {
        {"Sort by R", sortByR},
        {"Sort by D", sortByD},
        {"Schrage", schrage},
        {"Schrage Pr.", schragePreemptive},
        {"Vegas", vegasSort}
    };

    if (inst.size() <= 12) {
        algos.push_back({"Brute Force", bruteForce});
    }

    std::cout << std::left
              << std::setw(22) << "Algorithm"
              << std::setw(8)  << "Lmax"
              << std::setw(12) << "Time [us]" 
              << std::setw(10)  << "Error [%]" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    for (const auto& algo : algos) {
        
        auto start = std::chrono::high_resolution_clock::now();
        Solution sol = algo.func(inst);
        auto end = std::chrono::high_resolution_clock::now();
        double us = std::chrono::duration<double, std::micro>(end - start).count();

        results.push_back({algo.name, sol.lmax, us});
        lmaxs.push_back(sol.lmax);

        /*for (const auto& j : sol.schedule) {
            std::cout << j.id << " ";
        }*/
    }
    std::vector<double> errors = calculateError(lmaxs);
    for(int i = 0; i < results.size(); ++i){
        std::cout << std::left
                << std::setw(22) << results[i].name
                << std::setw(8)  << results[i].lmax
                << std::setw(12) << std::fixed << std::setprecision(1) << results[i].time_us
                << std::setw(10) << errors[i] << std::endl;
    }

    return 0;
}