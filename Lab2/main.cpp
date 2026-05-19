#include "FlowShopProblem.hh"
#include "InstanceGenerator.hh"
#include "Algorithms.hh"
#include <iostream>
#include <chrono>
#include <iomanip>

void runComparison() {
    std::cout << "Instance\tJohnson Cmax\tJohnson Time (ms)\t"
              << "BruteForce Cmax\tBruteForce Time (ms)\t"
              << "NEH Cmax\tNEH Time (ms)\n";
    std::cout << std::string(120, '-') << "\n";
    
    for (int n = 10; n <= 50; n += 10) {
        std::string filename = "../data/instance_" + std::to_string(n) + "x2.txt";
        
        FlowShopProblem problem = InstanceGenerator::generateAndSave(n, 2, 1, 99, filename);
        
        // Johnson
        auto start = std::chrono::high_resolution_clock::now();
        auto johnsonResult = Algorithms::johnson(problem);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> johnsonTime = end - start;
        
        // BruteForce only for small instances
        int bruteForceValue = 0;
        double bruteForceTimeMs = 0;
        if (n <= 12) {
            start = std::chrono::high_resolution_clock::now();
            auto bruteForceResult = Algorithms::bruteForce(problem);
            end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> bruteForceTime = end - start;
            bruteForceValue = bruteForceResult.second;
            bruteForceTimeMs = bruteForceTime.count();
        } else {
            bruteForceTimeMs = -1;
        }
        
        // NEH
        start = std::chrono::high_resolution_clock::now();
        auto nehResult = Algorithms::neh(problem);
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> nehTime = end - start;
        
        std::cout << n << "x2\t\t" 
                  << johnsonResult.second << "\t\t" << std::fixed << std::setprecision(3) << johnsonTime.count() << "\t\t\t";
        
        if (bruteForceTimeMs >= 0) {
            std::cout << bruteForceValue << "\t\t" << bruteForceTimeMs << "\t\t\t";
        } else {
            std::cout << "-\t\t-\t\t\t";
        }
        
        std::cout << nehResult.second << "\t\t" << nehTime.count() << "\n";
    }
}

int main() {
    std::cout << "--- Flow Shop Optimization Comparison ---\n\n";

    runComparison();

    return 0;
}