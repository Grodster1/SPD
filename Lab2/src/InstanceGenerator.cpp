#include "InstanceGenerator.hh"
#include <fstream>
#include <random>

FlowShopProblem InstanceGenerator::generateAndSave(int jobs, int machines, int minTime, int maxTime, const std::string& filename) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(minTime, maxTime);

    std::vector<std::vector<int>> times(jobs, std::vector<int>(machines));
    std::ofstream file(filename);

    if (file.is_open()) {
        file << jobs << " " << machines << "\n";
        for (int i = 0; i < jobs; ++i) {
            for (int j = 0; j < machines; ++j) {
                times[i][j] = distr(gen);
                file << times[i][j] << " ";
            }
            file << "\n";
        }
    }

    FlowShopProblem problem;
    problem.setTimes(jobs, machines, times);
    return problem;
}