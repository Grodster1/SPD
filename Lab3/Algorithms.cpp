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

Solution bruteForceP2(const Instance & inst){
    int n = inst.size();
    int bestCMax = INT_MAX;
    int load1, load2, cmax, bestMask;
    Solution sol(inst.m);
    //bitmask
    for(int i = 0; i < (1<<n); ++i){
        load1 = 0;
        load2 = 0;
        for(int bit = 0; bit < n; ++bit){
            if((i>>bit) & 1){
                load2 += inst.jobs[bit].p; 
            }
            else{
                load1 += inst.jobs[bit].p; 
            }
        }
        cmax = std::max(load1, load2);
        if(cmax < bestCMax){
            bestCMax = cmax;
            bestMask = i;
        } 
    }
    sol.assignment.resize(n);
    for(int bit = 0; bit < n; ++bit){
        sol.assignment[bit] = (bestMask >> bit) & 1;
    }
    sol.computeCMax(inst.jobs);
    return sol;
}

Solution ptasP2(const Instance & inst, double epsilon){
    int sumP = inst.totalP();
    int threshold = epsilon * sumP/2;

    int n = inst.size();
    int bestCMax = INT_MAX;
    int cmax, bestMask;

    Solution sol(inst.m);

    auto const condition_large = [threshold](const Job& j){return j.p > threshold;};
    auto const condition_small = [threshold](const Job& j){return j.p <= threshold;};

    std::vector<Job> largeJobs;
    std::vector<Job> smallJobs;

    std::copy_if(inst.jobs.begin(), inst.jobs.end(), std::back_inserter(largeJobs), condition_large);
    std::copy_if(inst.jobs.begin(), inst.jobs.end(), std::back_inserter(smallJobs), condition_small);

    std::vector<Job> sorted = smallJobs;
    std::sort(sorted.begin(), sorted.end(), [](const Job & a, const Job & b){
        return a.p > b.p;
    });

    for(int i = 0; i < (1<<largeJobs.size()); ++i){
        int loads[2] = {0, 0};
        for (int bit = 0; bit < (int)largeJobs.size(); ++bit) {
            loads[(i >> bit) & 1] += largeJobs[bit].p;
        }
        for (const auto& j : sorted) {
            int minM = (loads[0] <= loads[1]) ? 0 : 1;
            loads[minM] += j.p;
        }
    
        cmax = std::max(loads[0], loads[1]);
        if(cmax < bestCMax){
            bestCMax = cmax;
            bestMask = i;
        } 
    }

    sol.assignment.resize(n, -1);

    for (int bit = 0; bit < (int)largeJobs.size(); ++bit) {
        sol.assignment[largeJobs[bit].id] = (bestMask >> bit) & 1;
    }
    sol.loads.assign(2, 0);

    for (int bit = 0; bit < (int)largeJobs.size(); ++bit) {
        sol.loads[(bestMask >> bit) & 1] += largeJobs[bit].p;
    }

    for (const auto& j : sorted) {
        int minM = (sol.loads[0] <= sol.loads[1]) ? 0 : 1;
        sol.assign(j.id, j.p, minM);
    }

    return sol;

}


