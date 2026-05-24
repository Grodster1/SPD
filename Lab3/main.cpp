#include "Algorithms.h"
#include <iostream>
#include <chrono>
#include <functional>
#include <climits>
#include <iomanip>
#include <vector>
#include <algorithm>

struct Algorithm{
    std::string name;
    std::function<Solution(const Instance&)> func;
};

struct Results{
    std::string name;
    int cmax;
    double time_us;
};

std::vector<double> calculateError(const std::vector<int> cmaxs){
    int cmin = *std::min_element(cmaxs.begin(), cmaxs.end());
    std::vector<double> errors;
    for(int i = 0; i < cmaxs.size(); ++i){
        if (cmin == 0) {
            errors.push_back(0.0);
        } else {
            double error = (double)(cmaxs[i]-cmin)/abs(cmin)*100;
            errors.push_back(error);
        }
    }
    return errors;
}

int main(int argc, char* argv[]){
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

    std::vector<int> cmaxs;
    std::vector<Results> results;
    
    std::vector<Algorithm> algos = {
        Algorithm{"lsa", lsa},
        Algorithm{"lpt", lpt},
        Algorithm{"dynamicProg", dynamicProgP2},
        Algorithm{"ptasP2",  [](const Instance& i) { return ptasP2(i, 0.1); }},
        Algorithm{"fptasP2", [](const Instance& i) { return fptasP2(i, 0.1); }},
    };

    if (inst.size() <= 25) {
        algos.push_back({"bruteForce", bruteForceP2});
    }   
    
    std::cout << std::left
              << std::setw(22) << "Algorithm"
              << std::setw(8)  << "Cmax"
              << std::setw(12) << "Time [us]" 
              << std::setw(10)  << "Error [%]" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    for (const auto& algo : algos) {
        
        auto start = std::chrono::high_resolution_clock::now();
        Solution sol = algo.func(inst);
        auto end = std::chrono::high_resolution_clock::now();
        double us = std::chrono::duration<double, std::micro>(end - start).count();

        results.push_back({algo.name, sol.cmax, us});
        cmaxs.push_back(sol.cmax);

    }
    std::vector<double> errors = calculateError(cmaxs);
    for(int i = 0; i < results.size(); ++i){
        std::cout << std::left
                << std::setw(22) << results[i].name
                << std::setw(8)  << results[i].cmax
                << std::setw(12) << std::fixed << std::setprecision(1) << results[i].time_us
                << std::setw(10) << errors[i] << std::endl;
    }

    return 0;

}



