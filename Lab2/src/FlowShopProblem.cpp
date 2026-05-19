#include "FlowShopProblem.hh"
#include <fstream>
#include <algorithm>
#include <iostream>

bool FlowShopProblem::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    file >> numJobs >> numMachines;
    processingTimes.assign(numJobs, std::vector<int>(numMachines));

    for (int i = 0; i < numJobs; ++i) {
        for (int j = 0; j < numMachines; ++j) {
            file >> processingTimes[i][j];
        }
    }
    return true;
}

void FlowShopProblem::setTimes(int jobs, int machines, const std::vector<std::vector<int>>& times) {
    numJobs = jobs;
    numMachines = machines;
    processingTimes = times;
}

int FlowShopProblem::calculateCmax(const std::vector<int>& permutation) const {
    if (permutation.empty()) return 0;

    std::vector<int> completionTimes(numMachines, 0);

    for (int job : permutation) {
        completionTimes[0] += processingTimes[job][0];

        for (int m = 1; m < numMachines; ++m) {
            completionTimes[m] = std::max(completionTimes[m], completionTimes[m - 1]) + processingTimes[job][m];
        }
    }

    return completionTimes[numMachines - 1];
}