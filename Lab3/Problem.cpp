#include "Problem.h"
#include <fstream>
#include <iostream>
#include <climits>
#include <algorithm>
#include <random>
#include <numeric>
 
 
Instance Instance::fromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Can't open file: " << filename << std::endl;
        return {};
    }
 
    Instance inst;
    int n;
    file >> n >> inst.m;
    inst.jobs.resize(n);
 
    for (int i = 0; i < n; ++i) {
        inst.jobs[i].id = i;
        file >> inst.jobs[i].p;
    }
 
    return inst;
}
 
Instance Instance::fromFile(const std::string& filename, int n) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Can't open file: " << filename << std::endl;
        return {};
    }
 
    int total;
    file >> total >> std::ws;
 
    Instance inst;
    inst.m = 2;  // domyślnie P2
    inst.jobs.resize(std::min(n, total));
 
    for (int i = 0; i < (int)inst.jobs.size(); ++i) {
        inst.jobs[i].id = i;
        file >> inst.jobs[i].p;
    }
 
    return inst;
}


Instance Instance::generateRandom(int n, int m, int maxP){
    Instance inst;
    inst.m = m;
    inst.jobs.resize(n);

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(1, maxP);

    for(int i = 0; i < n; ++i){
        inst.jobs[i].id = i;
        inst.jobs[i].p = dist(rng);
    }

    return inst;
}

int Instance::totalP() const{
    int sum = 0;
    for(const auto & j: jobs){
        sum += j.p;
    }
    return sum;
}

void Instance::print() const {
    std::cout << "Instance: " << size() << " jobs, " << m << " machines" << std::endl;
    for (const auto& j : jobs) {
        std::cout << "  Job " << j.id << "  p=" << j.p << std::endl;
    }
}



Solution::Solution(int numMachines) : cmax(0), m(numMachines){
    loads.resize(numMachines, 0);
}

void Solution::assign(int JobID, int p, int machine){
    if((int)assignment.size() <= JobID){
        assignment.resize(JobID +1, -1);
    } 
    assignment[JobID] = machine;
    loads[machine] += p;
    cmax = *std::max_element(loads.begin(), loads.end());
}

void Solution::computeCMax(const std::vector<Job>& jobs) {
    loads.assign(m, 0);
    for (int j = 0; j < (int)jobs.size(); ++j) {
        int machine = assignment[j];
        if (machine >= 0 && machine < m) {
            loads[machine] += jobs[j].p;
        }
    }
    cmax = *std::max_element(loads.begin(), loads.end());
}
 
void Solution::print() const {
    std::cout << "Cmax = " << cmax << std::endl;
    for (int i = 0; i < m; ++i) {
        std::cout << "  Machine " << i << " (load=" << loads[i] << "): ";
        for (int j = 0; j < (int)assignment.size(); ++j) {
            if (assignment[j] == i) std::cout << j << " ";
        }
        std::cout << std::endl;
    }
}







