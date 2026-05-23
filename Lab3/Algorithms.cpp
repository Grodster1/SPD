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

Solution dynamicProgP2(const Instance & inst){
    int sumP = inst.totalP();
    int K = sumP / 2 + 1;
    int n = inst.size();

    Solution sol(inst.m);
    sol.assignment.resize(n, -1);

    std::vector<std::vector<int>> T(n+1, std::vector<int>(K,0));
    T[0][0]=1;
    for(int j = 1; j <= n; ++j){
        int pj = inst.jobs[j-1].p;
        for(int k = 0; k < K; ++k){
            if(T[j-1][k] == 1){
                T[j][k] = 1;  
            }
            if (k >= pj && T[j-1][k - pj] == 1) {
                T[j][k] = 1;  
            }
        }
    }

    int bestK = 0;
    for(int k =0; k < K; ++k){
        if(T[n][k]==1) bestK =k;
    }

    int k = bestK;
    for (int j = n; j >= 1; --j) {
        if (T[j-1][k] == 1) {
            sol.assignment[j-1] = 1;
        } else {
            sol.assignment[j-1] = 0;
            k -= inst.jobs[j-1].p;
        }
    }
    sol.computeCMax(inst.jobs);

    return sol;

}

