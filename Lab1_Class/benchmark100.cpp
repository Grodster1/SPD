#include "Algorithms.h"
#include <iostream>
#include <chrono>
#include <functional>
#include <climits>
#include <cmath>
#include <string>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <map>

struct Algorithm{
    std::string name;
    std::function<Solution(const Instance&)>func;
};

struct Stats {
    double avgError = 0;
    double avgTime = 0;
    int count = 0;
 
    void add(double error, double time_us) {
        avgError += error;
        avgTime += time_us;
        count++;
    }
 
    void summarize() {
        if (count > 0) {
            avgError /= count;
            avgTime /= count;
        }
    }
};


std::map<std::string, int> loadUBReference(const std::string& csvPath) {
    std::map<std::string, int> ubMap;
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        std::cerr << "Can't open UB reference: " << csvPath << std::endl;
        return ubMap;
    }
 
    std::string line;
    while (std::getline(file, line)) {
        auto comma = line.find(',');
        if (comma != std::string::npos) {
            std::string name = line.substr(0, comma);
            int ub = std::stoi(line.substr(comma + 1));
            ubMap[name] = ub;
        }
    }
 
    return ubMap;
}

std::string extractInstanceName(const std::string& filepath) {
    auto pos = filepath.find_last_of("/\\");
    std::string name = (pos != std::string::npos) ? filepath.substr(pos + 1) : filepath;
    auto dot = name.find_last_of('.');
    if (dot != std::string::npos) {
        name = name.substr(0, dot);
    }
    return name;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <folder> <ub_reference.csv> [num_files]" << std::endl;
        std::cerr << "Example: " << argv[0] << " ../files ub_reference.csv 50" << std::endl;
        return 1;
    }
 
    std::string folder = argv[1];
    std::string ubCsvPath = argv[2];
    int numFiles = (argc >= 4) ? std::atoi(argv[3]) : 9999;
 
    // Wczytaj referencyjne UB
    auto ubMap = loadUBReference(ubCsvPath);
    std::cout << "Loaded " << ubMap.size() << " UB reference values" << std::endl;
 
    // Zbierz pliki
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
    std::sort(files.begin(), files.end());
 
    if ((int)files.size() > numFiles) {
        files.resize(numFiles);
    }
 
    std::cout << "Found " << files.size() << " files in " << folder << std::endl;
 
    // Algorytmy
    std::vector<Algorithm> algos = {
        {"Sort r_j",     sortByR},
        {"Sort d_j",     sortByD},
        {"Schrage",      schrage},
        {"Schrage Pr.",  schragePreemptive},
        {"Vegas",        vegasSort},
    };
 
    int numAlgos = algos.size();
    std::vector<Stats> stats(numAlgos);
 
    int filesProcessed = 0;
    int filesSkipped = 0;
 
    for (const auto& filepath : files) {
        Instance inst = Instance::fromFile(filepath);
        if (inst.size() == 0) continue;
 
        std::string instName = extractInstanceName(filepath);
        auto it = ubMap.find(instName);
        if (it == ubMap.end()) {
            filesSkipped++;
            continue;
        }
        int ub = it->second;
 
        for (int i = 0; i < numAlgos; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            Solution sol = algos[i].func(inst);
            auto end = std::chrono::high_resolution_clock::now();
            double us = std::chrono::duration<double, std::micro>(end - start).count();
 
            double error = 0;
            if (ub != 0) {
                error = (double)(sol.lmax - ub) / std::abs(ub) * 100.0;
            }
 
            stats[i].add(error, us);
        }
 
        filesProcessed++;
    }
 
    // Finalizuj
    for (auto& s : stats) {
        s.summarize();
    }
 
    // Wypisz wyniki
    std::cout << "\n========== n = 100 (benchmark) ==========" << std::endl;
    std::cout << "Instances: " << filesProcessed
              << " | Skipped: " << filesSkipped
              << " | Reference: UB from benchmark" << std::endl;
 
    std::cout << std::left
              << std::setw(16) << "Algorithm"
              << std::setw(12) << "Avg.err[%]"
              << std::setw(14) << "Avg.time[us]"
              << std::endl;
    std::cout << std::string(42, '-') << std::endl;
 
    for (int i = 0; i < numAlgos; ++i) {
        std::cout << std::left
                  << std::setw(16) << algos[i].name
                  << std::setw(12) << std::fixed << std::setprecision(2) << stats[i].avgError
                  << std::setw(14) << std::setprecision(1) << stats[i].avgTime
                  << std::endl;
    }
 
    return 0;
}

