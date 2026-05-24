#include "Algorithms.h"
#include <iostream>
#include <chrono>
#include <functional>
#include <iomanip>
#include <climits>
#include <cmath>
#include <vector>
#include <random>

struct Algorithm {
    std::string name;
    std::function<Solution(const Instance&)> func;
};

struct Stats {
    double avgCmax = 0;
    double avgError = 0;
    double avgTime = 0;
    int count = 0;

    void add(int cmax, double error, double time_us) {
        avgCmax += cmax;
        avgError += error;
        avgTime += time_us;
        count++;
    }

    void summarize() {
        if (count > 0) {
            avgCmax /= count;
            avgError /= count;
            avgTime /= count;
        }
    }
};

struct Config {
    int m;
    int n;
    int minP;
    int maxP;
};

// Generuj losową instancję z kontrolowanym przedziałem p_j
Instance generateInstance(int n, int m, int minP, int maxP) {
    Instance inst;
    inst.m = m;
    inst.jobs.resize(n);

    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(minP, maxP);

    for (int i = 0; i < n; ++i) {
        inst.jobs[i].id = i;
        inst.jobs[i].p = dist(rng);
    }

    return inst;
}

int main(int argc, char* argv[]) {
    int numInstances = 50;
    if (argc >= 2) {
        numInstances = std::atoi(argv[1]);
    }

    std::vector<Config> configs = {
        {2, 10, 1, 10},
        {2, 10, 10, 20},
        {2, 20, 1, 10},
        {2, 20, 10, 20},
        {2, 20, 50, 100},
        {2, 50, 1, 10},
        {2, 50, 10, 20},
        {2, 50, 50, 100},
    };

    // ============================================================
    // TABELA 1: LSA, LPT, DP, Brute Force
    // ============================================================
    std::cout << "============================================" << std::endl;
    std::cout << "TABELA 1: LSA, LPT, DP, Brute Force" << std::endl;
    std::cout << "Instances per config: " << numInstances << std::endl;
    std::cout << "============================================\n" << std::endl;

    std::cout << std::left
              << std::setw(20) << "Config"
              << std::setw(16) << "LSA Cmax[err%]"
              << std::setw(12) << "LSA t[us]"
              << std::setw(16) << "LPT Cmax[err%]"
              << std::setw(12) << "LPT t[us]"
              << std::setw(12) << "DP Cmax"
              << std::setw(12) << "DP t[us]"
              << std::setw(12) << "BF t[us]"
              << std::endl;
    std::cout << std::string(112, '-') << std::endl;

    for (const auto& cfg : configs) {
        std::vector<Algorithm> algos = {
            {"LSA", lsa},
            {"LPT", lpt},
            {"DP",  dynamicProgP2},
        };

        bool useBF = (cfg.n <= 25);

        int numAlgos = algos.size();
        std::vector<Stats> stats(numAlgos);
        Stats bfStats;
        double avgOptimum = 0;

        for (int i = 0; i < numInstances; ++i) {
            Instance inst = generateInstance(cfg.n, cfg.m, cfg.minP, cfg.maxP);

            // Optimum z DP
            auto startOpt = std::chrono::high_resolution_clock::now();
            Solution optSol = dynamicProgP2(inst);
            auto endOpt = std::chrono::high_resolution_clock::now();
            int optimum = optSol.cmax;
            avgOptimum += optimum;

            // Brute force (osobno mierzymy czas)
            if (useBF) {
                auto startBF = std::chrono::high_resolution_clock::now();
                Solution bfSol = bruteForceP2(inst);
                auto endBF = std::chrono::high_resolution_clock::now();
                double bfUs = std::chrono::duration<double, std::micro>(endBF - startBF).count();
                bfStats.add(bfSol.cmax, 0, bfUs);
            }

            for (int a = 0; a < numAlgos; ++a) {
                auto start = std::chrono::high_resolution_clock::now();
                Solution sol = algos[a].func(inst);
                auto end = std::chrono::high_resolution_clock::now();
                double us = std::chrono::duration<double, std::micro>(end - start).count();

                double error = 0;
                if (optimum != 0) {
                    error = (double)(sol.cmax - optimum) / std::abs(optimum) * 100.0;
                }

                stats[a].add(sol.cmax, error, us);
            }
        }

        for (auto& s : stats) s.summarize();
        bfStats.summarize();
        avgOptimum /= numInstances;

        std::string label = std::to_string(cfg.m) + "/" + std::to_string(cfg.n)
                          + " [" + std::to_string(cfg.minP) + "-" + std::to_string(cfg.maxP) + "]";

        std::cout << std::left << std::fixed
                  << std::setw(20) << label
                  << std::setprecision(1)
                  << std::setw(16) << (std::to_string((int)stats[0].avgCmax) + " [" +
                                       std::to_string(stats[0].avgError).substr(0,4) + "%]")
                  << std::setw(12) << stats[0].avgTime
                  << std::setw(16) << (std::to_string((int)stats[1].avgCmax) + " [" +
                                       std::to_string(stats[1].avgError).substr(0,4) + "%]")
                  << std::setw(12) << stats[1].avgTime
                  << std::setw(12) << (int)stats[2].avgCmax
                  << std::setw(12) << stats[2].avgTime;

        if (useBF) {
            std::cout << std::setw(12) << bfStats.avgTime;
        } else {
            std::cout << std::setw(12) << "---";
        }
        std::cout << std::endl;
    }

    // ============================================================
    // TABELA 2: PTAS i FPTAS z różnymi parametrami
    // ============================================================
    std::cout << "\n============================================" << std::endl;
    std::cout << "TABELA 2: PTAS i FPTAS" << std::endl;
    std::cout << "============================================\n" << std::endl;

    // PTAS: K = n/2, 2n/3, 3n/4  -> epsilon = 2/K * n ... 
    // Wzór mówi o K jako liczbie dużych zadań do brute force.
    // W naszej implementacji kontrolujemy epsilon:
    //   K=n/2  -> bierzemy n/2 największych jako "duże" -> epsilon ≈ 2*sum/(n*sum) ...
    // Prostsze: testujemy epsilon = 0.5, 0.33, 0.25 (odpowiada K ≈ n/2, 2n/3, 3n/4)
    
    struct PtasConfig {
        std::string name;
        double epsilon;
    };

    std::vector<PtasConfig> ptasConfigs = {
        {"PTAS e=0.5",  0.5},
        {"PTAS e=0.33", 0.33},
        {"PTAS e=0.25", 0.25},
    };

    struct FptasConfig {
        std::string name;
        double epsilon;
    };

    std::vector<FptasConfig> fptasConfigs = {
        {"FPTAS e=0.5",  0.5},
        {"FPTAS e=0.33", 0.33},
        {"FPTAS e=0.25", 0.25},
    };

    // Nagłówek
    std::cout << std::left << std::setw(20) << "Config";
    for (const auto& pc : ptasConfigs) {
        std::cout << std::setw(12) << (pc.name + " err") << std::setw(10) << "t[us]";
    }
    for (const auto& fc : fptasConfigs) {
        std::cout << std::setw(12) << (fc.name + " err") << std::setw(10) << "t[us]";
    }
    std::cout << std::endl;
    std::cout << std::string(20 + (ptasConfigs.size() + fptasConfigs.size()) * 22, '-') << std::endl;

    for (const auto& cfg : configs) {
        int totalParams = ptasConfigs.size() + fptasConfigs.size();
        std::vector<Stats> stats(totalParams);

        for (int i = 0; i < numInstances; ++i) {
            Instance inst = generateInstance(cfg.n, cfg.m, cfg.minP, cfg.maxP);
            int optimum = dynamicProgP2(inst).cmax;

            // PTAS
            for (int p = 0; p < (int)ptasConfigs.size(); ++p) {
                auto start = std::chrono::high_resolution_clock::now();
                Solution sol = ptasP2(inst, ptasConfigs[p].epsilon);
                auto end = std::chrono::high_resolution_clock::now();
                double us = std::chrono::duration<double, std::micro>(end - start).count();

                double error = (optimum != 0)
                    ? (double)(sol.cmax - optimum) / std::abs(optimum) * 100.0
                    : 0;
                stats[p].add(sol.cmax, error, us);
            }

            // FPTAS
            for (int f = 0; f < (int)fptasConfigs.size(); ++f) {
                auto start = std::chrono::high_resolution_clock::now();
                Solution sol = fptasP2(inst, fptasConfigs[f].epsilon);
                auto end = std::chrono::high_resolution_clock::now();
                double us = std::chrono::duration<double, std::micro>(end - start).count();

                double error = (optimum != 0)
                    ? (double)(sol.cmax - optimum) / std::abs(optimum) * 100.0
                    : 0;
                int idx = ptasConfigs.size() + f;
                stats[idx].add(sol.cmax, error, us);
            }
        }

        for (auto& s : stats) s.summarize();

        std::string label = std::to_string(cfg.m) + "/" + std::to_string(cfg.n)
                          + " [" + std::to_string(cfg.minP) + "-" + std::to_string(cfg.maxP) + "]";

        std::cout << std::left << std::fixed << std::setprecision(2)
                  << std::setw(20) << label;

        for (int j = 0; j < totalParams; ++j) {
            std::string errStr = std::to_string(stats[j].avgError).substr(0, 5) + "%";
            std::cout << std::setw(12) << errStr
                      << std::setw(10) << std::setprecision(1) << stats[j].avgTime;
        }
        std::cout << std::endl;
    }

    return 0;
}