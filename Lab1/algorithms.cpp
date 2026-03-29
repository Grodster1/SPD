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

