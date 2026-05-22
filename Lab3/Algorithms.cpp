#include "Algorithims.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include <queue>
#include <cmath>
#include <numeric>
#include <climits>


Solution lsa(const Instance & inst){
    Solution sol(inst.m);

    for(const auto& j : inst.jobs){
        auto it = std::min_element(sol.loads.begin(), sol.loads.end());
        int minMachine = std::distance(sol.loads.begin(), it);
        sol.assign(j.id, j.p, minMachine);
    }
    return sol;
}

Solution lpt(const Instance & inst){
    std::vector<Job> sorted = inst.jobs;
    std::sort(sorted.begin(), sorted.end(), [](const Job & a, const Job & b){
        return a.p > b.p;
    });

    Solution sol(inst.m);

    for(const auto& j : sorted){
        auto it = std::min_element(sol.loads.begin(), sol.loads.end());
        int minMachine = std::distance(sol.loads.begin(), it);
        sol.assign(j.id, j.p, minMachine);
    }
    return sol;
}

