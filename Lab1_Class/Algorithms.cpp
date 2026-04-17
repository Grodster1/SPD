#include "Algorithms.h"
#include <algorithm>
#include <climits>
#include <iostream>
#include <random>
#include <queue>

Solution sortByR(const Instance& inst){
    Solution sol;
    sol.schedule = inst.jobs;
    std::sort(sol.schedule.begin(), sol.schedule.end(), [](const Job& a, const Job& b){
        return a.r < b.r;
    });

    sol.computeLmax();
    return sol;
}

Solution sortByD(const Instance& inst){
    Solution sol;
    sol.schedule = inst.jobs;
    std::sort(sol.schedule.begin(), sol.schedule.end(), [](const Job& a, const Job& b){
        return a.d < b.d;
    });
     
    sol.computeLmax();
    return sol;
}

Solution bruteForce(const Instance& inst){
    Solution best;
    best.lmax = INT_MAX;

    std::vector<Job> perm = inst.jobs;
    std::sort(perm.begin(), perm.end(), [](const Job& a, const Job& b){\
    return a.id < b.id;});
    do{
        Solution current;
        current.schedule = perm;
        current.computeLmax();

        if(current.lmax < best.lmax){
            best = current;
        }
    } while(std::next_permutation(perm.begin(), perm.end(), [](const Job&a, const Job&b){
        return a.id < b.id;
    }));

    return best;

}

Solution schrage(const Instance & inst){
    std::vector<Job> N = inst.jobs;
    std::sort(N.begin(), N.end(), [](const Job& a, const Job& b){
        return a.r < b.r;
    });

    auto comp = [](const Job& a, const Job& b){return a.d > b.d;}; //odwrotnie bo domyślnie jest max-heap
    std::priority_queue<Job, std::vector<Job>, decltype(comp)> G(comp);
    int t=0;
    int lmax = INT_MIN;
    int idx = 0;
    int n = N.size();

    //std::vector<Job> G;
    Solution sol;

    while(idx < n || !G.empty()){
        while (idx < n && N[idx].r <= t) {
            G.push(N[idx]);
            idx++;
        }
 
        if (G.empty()) {
            t = N[idx].r;
            continue;
        }
 
        Job best = G.top();
        G.pop();
 
        sol.schedule.push_back(best);
        t += best.p;
        int L = t - best.d;
        lmax = std::max(lmax, L);
    }
 
    sol.lmax = lmax;
    return sol;
}

Solution schragePreemptive(const Instance & inst){
    std::vector<Job> N = inst.jobs;
    std::sort(N.begin(), N.end(), [](const Job& a, const Job& b){
        return a.r < b.r;
    });

    auto comp = [](const Job& a, const Job& b){return a.d > b.d;}; 
    std::priority_queue<Job, std::vector<Job>, decltype(comp)> G(comp);
    int t=0;
    int lmax = INT_MIN;
    int idx = 0;
    int n = N.size();

    Solution sol;

    while(idx < n || !G.empty()){
        while (idx < n && N[idx].r <= t) {
            G.push(N[idx]);
            idx++;
        }
 
        if (G.empty()) {
            t = N[idx].r;
            continue;
        }
 
        Job best = G.top();
        G.pop();
 
        sol.schedule.push_back(best);
        int r_next = (idx < n) ? N[idx].r : INT_MAX;
        int t_end = t + best.p;
        int t_new = std::min(t_end, r_next);

        best.p -= (t_new - t);
        t = t_new;

        if(best.p == 0){
            int L = t-best.d;
            lmax = std::max(lmax, L);
        }
        else{
            G.push(best);
        }
    }
 
    sol.lmax = lmax;
    return sol;
}

Solution vegasSort(const Instance& inst){
    std::vector<Job> perm = inst.jobs;
    Solution best;
    best.schedule = perm;
    best.computeLmax();
    for(int i = 0; i < inst.jobs.size(); ++i){
        std::shuffle(perm.begin(), perm.end(), std::mt19937{std::random_device{}()});
        Solution current;
        current.schedule = perm;
        current.computeLmax();
        if(current.lmax < best.lmax){
            best=current;
        }
    }
    return best;
}

