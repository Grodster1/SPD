#include "algorithms.h"
#include <algorithm>
#include <limits.h>
#include <iostream>


int sortByR(std::vector<Job>& jobs){

    std::sort(jobs.begin(), jobs.end(), [](const Job& a, const Job& b){
        return a.r < b.r;
    });
    return computeLMax(jobs);
}

int sortByD(std::vector<Job>& jobs){
    std::sort(jobs.begin(), jobs.end(), [](const Job& a, const Job& b){
        return a.d < b.d;
    });
    return computeLMax(jobs);
}

std::pair<std::vector<Job>, int> bruteForce(std::vector<Job>& jobs){
    int lmax = INT_MAX;
    std::vector<Job> bestPerm;
    do{
        for(auto j:jobs){
            std::cout << j.id << ' ';
        }
        std::cout << computeLMax(jobs) << std::endl;
        if(lmax > computeLMax(jobs)){
            lmax = computeLMax(jobs);
            bestPerm = jobs;
        }
        lmax = std::min(computeLMax(jobs), lmax);
    } while(std::next_permutation(jobs.begin(), jobs.end(), [](const Job& a, const Job& b){
        return a.id < b.id;
    }));

    return std::pair(bestPerm, lmax); 
}

std::pair<std::vector<Job>,int> sortSchrage(std::vector<Job> jobs){
    
    std::sort(jobs.begin(), jobs.end(), [](const Job& a, const Job& b){
        return a.r < b.r;
    });
    int t = 0;
    int lmax = INT_MIN;
    int idx = 0;
    int n = jobs.size();

    std::vector<Job> available, permutation;
    

    while(idx < n || !available.empty()){
        while(idx < n && jobs[idx].r <= t){
            available.push_back(jobs[idx]);
            idx ++;
        }
        if(available.empty()){
            t = jobs[idx].r;
            continue;
        }

        auto min = std::min_element(available.begin(), available.end(), [](const Job& a, const Job& b){
            return a.d < b.d;
        });
        permutation.emplace_back(*min);
        t += min->p;
        int L = t - min->d;
        lmax = std::max(lmax, L);
        available.erase(min);
    }
    return std::pair(permutation, lmax);
}

