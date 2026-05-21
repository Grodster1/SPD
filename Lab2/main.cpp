#include "FlowShopProblem.hh"
#include "InstanceGenerator.hh"
#include "Algorithms.hh"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>

void runComparison() {
    std::cout << "Instance\tJohnson Cmax\tJohnson (ms)\t"
              << "M1 Cmax\tM1 (ms)\t"
              << "NEH Cmax\tNEH (ms)\t"
              << "B&B Cmax\tB&B (ms)\t"
              << "BruteForce Cmax\tBruteForce (ms)\n";
    std::cout << std::string(140, '-') << "\n";
    
    // Open output file
    std::ofstream outFile("../results.txt");
    if (!outFile.is_open()) {
        std::cerr << "Failed to open results.txt for writing\n";
        return;
    }
    
    // Write header
    outFile << "Instance\tAlgorithm\tCmax\tOptimal\tRelativeError(%)\tTime(ms)\n";
    outFile << std::string(80, '-') << "\n";
    
    std::vector<int> instances = {10, 20, 50, 100, 500, 1000};
    bool firstInstance = true;
    
    for (int n : instances) {
        std::string filename = "../data/instance_" + std::to_string(n) + "x2.txt";
        
        FlowShopProblem problem = InstanceGenerator::generateAndSave(n, 2, 1, 99, filename);
        
        // Determine optimal value (use brute force for small instances, otherwise use best algorithm)
        int optimalValue = 0;
        double bruteForceTimeMs = 0;
        
        if (firstInstance) {
            auto start = std::chrono::high_resolution_clock::now();
            auto bruteForceResult = Algorithms::bruteForce(problem);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> bruteForceTime = end - start;
            optimalValue = bruteForceResult.second;
            bruteForceTimeMs = bruteForceTime.count();
            
            outFile << n << "x2\tBruteForce\t" << optimalValue << "\t" << optimalValue 
                   << "\t0.000\t" << std::fixed << std::setprecision(3) << bruteForceTimeMs << "\n";
        } else {
            // For larger instances, use NEH as reference (good heuristic)
            auto nehResult = Algorithms::neh(problem);
            optimalValue = nehResult.second;
        }
        
        // Johnson
        auto start = std::chrono::high_resolution_clock::now();
        auto johnsonResult = Algorithms::johnson(problem);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> johnsonTime = end - start;
        double johnsonError = (optimalValue > 0) ? 
            (100.0 * (johnsonResult.second - optimalValue) / optimalValue) : 0;
        
        outFile << n << "x2\tJohnson\t" << johnsonResult.second << "\t" << optimalValue 
               << "\t" << std::fixed << std::setprecision(3) << johnsonError << "\t" 
               << johnsonTime.count() << "\n";
        
        // M1
        start = std::chrono::high_resolution_clock::now();
        auto m1Result = Algorithms::m1(problem);
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> m1Time = end - start;
        double m1Error = (optimalValue > 0) ? 
            (100.0 * (m1Result.second - optimalValue) / optimalValue) : 0;
        
        outFile << n << "x2\tM1\t" << m1Result.second << "\t" << optimalValue 
               << "\t" << std::fixed << std::setprecision(3) << m1Error << "\t" 
               << m1Time.count() << "\n";
        
        // NEH
        start = std::chrono::high_resolution_clock::now();
        auto nehResult = Algorithms::neh(problem);
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> nehTime = end - start;
        double nehError = (optimalValue > 0) ? 
            (100.0 * (nehResult.second - optimalValue) / optimalValue) : 0;
        
        outFile << n << "x2\tNEH\t" << nehResult.second << "\t" << optimalValue 
               << "\t" << std::fixed << std::setprecision(3) << nehError << "\t" 
               << nehTime.count() << "\n";
        
        // Branch & Bound
        start = std::chrono::high_resolution_clock::now();
        auto bbResult = Algorithms::branchAndBound(problem);
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> bbTime = end - start;
        double bbError = (optimalValue > 0) ? 
            (100.0 * (bbResult.second - optimalValue) / optimalValue) : 0;
        
        outFile << n << "x2\tB&B\t" << bbResult.second << "\t" << optimalValue 
               << "\t" << std::fixed << std::setprecision(3) << bbError << "\t" 
               << bbTime.count() << "\n";
        
        // Console output
        std::cout << n << "x2\t\t" 
                  << johnsonResult.second << "\t\t" << std::fixed << std::setprecision(3) << johnsonTime.count() << "\t\t"
                  << m1Result.second << "\t\t" << m1Time.count() << "\t\t"
                  << nehResult.second << "\t\t" << nehTime.count() << "\t\t"
                  << bbResult.second << "\t\t" << bbTime.count() << "\t\t";
        
        if (firstInstance) {
            std::cout << optimalValue << "\t\t" << bruteForceTimeMs << "\n";
        } else {
            std::cout << "-\t\t-\n";
        }
        
        outFile << "\n";
        firstInstance = false;
    }
    
    outFile.close();
    std::cout << "\nResults saved to ../results.txt\n";
}

int main() {
    std::cout << "--- Flow Shop Optimization Comparison ---\n\n";

    runComparison();
    
    // Test with multiple machine instances
    std::cout << "\n--- Testing instances with m > 2 machines ---\n\n";
    
    std::ofstream outFile("../results_multi_machine.txt");
    if (!outFile.is_open()) {
        std::cerr << "Failed to open results_multi_machine.txt for writing\n";
        return 1;
    }
    
    outFile << "Instance\tAlgorithm\tCmax\tTime(ms)\n";
    outFile << std::string(60, '-') << "\n";
    
    std::vector<std::pair<int, int>> multiMachineInstances = {{20, 5}, {20, 10}, {50, 10}};
    
    for (const auto& config : multiMachineInstances) {
        int n = config.first;
        int m = config.second;
        std::string filename = "../data/instance_" + std::to_string(n) + "x" + std::to_string(m) + ".txt";
        
        FlowShopProblem problem = InstanceGenerator::generateAndSave(n, m, 1, 99, filename);
        
        std::cout << "Testing " << n << "x" << m << " instance:\n";
        outFile << "\n" << n << "x" << m << " instance:\n";
        
        // M1
        auto start = std::chrono::high_resolution_clock::now();
        auto m1Result = Algorithms::m1(problem);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> m1Time = end - start;
        
        std::cout << "  M1:   Cmax = " << m1Result.second << ", Time = " << std::fixed << std::setprecision(3) << m1Time.count() << " ms\n";
        outFile << n << "x" << m << "\tM1\t" << m1Result.second << "\t" << std::fixed << std::setprecision(3) << m1Time.count() << "\n";
        
        // NEH
        start = std::chrono::high_resolution_clock::now();
        auto nehResult = Algorithms::neh(problem);
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> nehTime = end - start;
        
        std::cout << "  NEH:  Cmax = " << nehResult.second << ", Time = " << std::fixed << std::setprecision(3) << nehTime.count() << " ms\n";
        outFile << n << "x" << m << "\tNEH\t" << nehResult.second << "\t" << std::fixed << std::setprecision(3) << nehTime.count() << "\n";
        
        // QNEH
        start = std::chrono::high_resolution_clock::now();
        auto qnehResult = Algorithms::qneh(problem);
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> qnehTime = end - start;
        
        std::cout << "  QNEH: Cmax = " << qnehResult.second << ", Time = " << std::fixed << std::setprecision(3) << qnehTime.count() << " ms\n";
        outFile << n << "x" << m << "\tQNEH\t" << qnehResult.second << "\t" << std::fixed << std::setprecision(3) << qnehTime.count() << "\n";
        
        // Branch & Bound
        start = std::chrono::high_resolution_clock::now();
        auto bbResult = Algorithms::branchAndBound(problem);
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> bbTime = end - start;
        
        std::cout << "  B&B:  Cmax = " << bbResult.second << ", Time = " << std::fixed << std::setprecision(3) << bbTime.count() << " ms\n";
        outFile << n << "x" << m << "\tB&B\t" << bbResult.second << "\t" << std::fixed << std::setprecision(3) << bbTime.count() << "\n";
    }
    
    outFile.close();
    std::cout << "\nResults saved to ../results_multi_machine.txt\n";

    return 0;
}