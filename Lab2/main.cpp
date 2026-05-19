#include "FlowShopProblem.hh"
#include "InstanceGenerator.hh"
#include "Algorithms.hh"
#include <iostream>
#include <chrono>

void runExperimentForJohnson() {
    std::cout << "Rozmiar\tCzas działania (ms)\tCmax\n";
    
    // Pętla badawcza: od 20 do 920 zadań dla m=2 (z krokiem 100)
    for (int n = 20; n <= 920; n += 100) {
        std::string filename = "../data/instance_" + std::to_string(n) + "x2.txt";
        
        // Generujemy instancję
        FlowShopProblem problem = InstanceGenerator::generateAndSave(n, 2, 1, 99, filename);
        
        // Pomiar czasu
        auto start = std::chrono::high_resolution_clock::now();
        auto result = Algorithms::johnson(problem);
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::milli> duration = end - start;
        
        std::cout << n << "x2\t" << duration.count() << " ms\t\t" << result.second << "\n";
    }
}

int main() {
    std::cout << "--- Rozpoczecie badan operacyjnych (Flow Shop) ---\n\n";

    runExperimentForJohnson();

    return 0;
}