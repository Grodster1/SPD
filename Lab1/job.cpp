#include "job.h"
#include <iostream>
#include <fstream>
#include <limits.h>

int computeLMax(const std::vector<Job>& schedule){
    int t = 0;
    int lmax = INT_MIN;
    for (const auto& j:schedule){
        t = std::max(t, j.r);
        t += j.p;
        int L = t-j.d;
        lmax = std::max(L, lmax);
    }
    return lmax;
}

std::vector<Job> loadData(const std::string& fileName){
    std::ifstream file(fileName);
    if(!file.is_open()){
        std::cerr << "Can't open the file: " << fileName << std::endl;
        return {};
    }

    int n;
    file >> n;
    std::vector<Job> jobs(n);

    for(int i = 0; i < n; ++i){
        jobs[i].id = i;
        file >> jobs[i].r >> jobs[i].p >> jobs[i].d;
    }

    return jobs;

}