#ifndef FLOW_SHOP_PROBLEM_HH
#define FLOW_SHOP_PROBLEM_HH

#include <vector>
#include <string>

class FlowShopProblem {
private:
    int numJobs;
    int numMachines;
    std::vector<std::vector<int>> processingTimes; 

public:
    FlowShopProblem() : numJobs(0), numMachines(0) {}
    
    bool loadFromFile(const std::string& filename);
    
    // Calculates Cmax for the given permutation
    int calculateCmax(const std::vector<int>& permutation) const;

    // Getters
    int getNumJobs() const { return numJobs; }
    int getNumMachines() const { return numMachines; }
    int getDuration(int jobIdx, int machineIdx) const { return processingTimes[jobIdx][machineIdx]; }
    
    // Setter
    void setTimes(int jobs, int machines, const std::vector<std::vector<int>>& times);
};

#endif