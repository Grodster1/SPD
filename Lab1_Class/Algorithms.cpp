#include <Algorithms.h>
#include <algorithm>
#include <climits>
#include <iostream>

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

    int t=0;
    int lmax = INT_MIN;
    int idx = 0;
    int n = N.size();
    std::vector<Job> G;
    Solution sol;

    while(idx < n || G.empty()){
        while (idx < n && N[idx].r <= t) {
            G.push_back(N[idx]);
            idx++;
        }
 
        if (G.empty()) {
            t = N[idx].r;
            continue;
        }
 
        auto it = std::min_element(G.begin(), G.end(),
            [](const Job& a, const Job& b) { return a.d < b.d; });
 
        sol.schedule.push_back(*it);
        t += it->p;
        int L = t - it->d;
        lmax = std::max(lmax, L);
        G.erase(it);
    }
 
    sol.lmax = lmax;
    return sol;
}