#include "Algorithms.h"
#include <iostream>
#include <chrono>
#include <functional>
#include <iomanip>
#include <climits>
#include <cmath>
#include <string>
#include <filesystem>
 

struct Algorithm{
    std::string name;
    std::function<Solution(const Instance&)> func;
    
};

struct Stats{
    double avgError = 0;
    double avgTime = 0;
    int count = 0;
    
    void add(double error, double time_us){
        avgError += error;
        avgTime += time_us;
        count++;
    }

    void summarize(){
        if(count > 0){
            avgError /= count;
            avgTime /= count;
        }
    }
};

int main(int argc, char* argv[]){
    if(argc < 3){
        std::cerr << "Usage: " << argv[0] << " <folder> <num_files>" << std::endl;
        return 1;
    }

    std::string folder = argv[1];
    int num_files = std::atoi(argv[2]);

    std::vector<int> instances = {5,7,9,10,11,12,20,50};
    std::vector<std::string> files;

    for(const auto& f: std::filesystem::directory_iterator(folder)){
        if(f.is_regular_file()){
            files.push_back(f.path().string());
        }
    }
    std::sort(files.begin(), files.end());
    if(files.size() > num_files){
        files.resize(num_files);
    }

    std::cout << "Found " << files.size() << " files in " << folder << " directory"<< std::endl;

    std::vector<Algorithm> algos = {
        {"Sort r_j", sortByR},
        {"Sort d_j", sortByD},
        {"Schrage", schrage},
        {"Schrage pr.", schragePreemptive},
        {"Vegas", vegasSort}
    };

    for(int n : instances){
        std::cout << "\n========== n = " << n << " ==========" << std::endl;
        bool useBruteForce=false;
        if(n <= 12){
            useBruteForce=true;
        }
        std::vector<Algorithm> activeAlgos = algos;
        if(useBruteForce){
            activeAlgos.push_back({"Brute Force", bruteForce});
        }
        int numAlgos = activeAlgos.size();
        std::vector<Stats> stats(numAlgos);
 
        int filesProcessed = 0;

        for (const auto& filepath : files) {
            Instance inst = Instance::fromFile(filepath, n);
            if (inst.size() == 0) continue;
 
            int optimum;
            if (useBruteForce) {
                optimum = bruteForce(inst).lmax;
            } 
            else {
                optimum = schragePreemptive(inst).lmax;
            }
 
            for (int i = 0; i < numAlgos; ++i) {
                auto start = std::chrono::high_resolution_clock::now();
                Solution sol = activeAlgos[i].func(inst);
                auto end = std::chrono::high_resolution_clock::now();
                double us = std::chrono::duration<double, std::micro>(end - start).count();
 
                double error = 0;
                if (optimum != 0) {
                    error = (double)(sol.lmax - optimum) / std::abs(optimum) * 100.0;
                }
 
                stats[i].add(error, us);
            }
 
            filesProcessed++;
        }
 
        for (auto& s : stats) {
            s.summarize();
        }
 
        std::string refLabel = useBruteForce ? "Brute Force" : "Schrage Pr.";
        std::cout << "Plików: " << filesProcessed
                  << " | Reference: " << refLabel << std::endl;
 
        std::cout << std::left
                  << std::setw(16) << "Algorithm"
                  << std::setw(12) << "Avg.err[%]"
                  << std::setw(14) << "Avg.time[us]"
                  << std::endl;
        std::cout << std::string(40, '-') << std::endl;
 
        for (int i = 0; i < numAlgos; ++i) {
            std::cout << std::left
                      << std::setw(16) << activeAlgos[i].name
                      << std::setw(12) << std::fixed << std::setprecision(2) << stats[i].avgError
                      << std::setw(14) << std::setprecision(1) << stats[i].avgTime
                      << std::endl;
        }
    }

}

